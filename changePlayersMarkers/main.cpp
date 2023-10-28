#include "plugin.h"
#include "CRadar.h"
#include "CNetGame.h"
#include "CRemotePlayer.h"
#include <kthook/kthook.hpp>

using namespace plugin;

namespace samp = sampapi::v037r1;

using CTimer__UpdateSignature = void(__cdecl*)();
using CRadar__DrawBlipsSignature = void(__cdecl*)();
kthook::kthook_simple<CTimer__UpdateSignature> CTimerHook{};
kthook::kthook_simple<CRadar__DrawBlipsSignature> CRadarHook{};

void CRadar__DrawBlips(const decltype(CRadarHook)& hook) {
    hook.get_trampoline()();
    for (int i = 0; i < samp::CPlayerPool::MAX_PLAYERS; i++) {
        samp::CPlayerPool* pool = samp::RefNetGame()->GetPlayerPool();
        if (pool->IsConnected(i)) {
            samp::CRemotePlayer* player = pool->GetPlayer(i);
            if (player->DoesExist()) {
                CPed* pPed = player->m_pPed->m_pGamePed;
                if (pPed != nullptr) {
                    CVector2D pedPosition = pPed->GetPosition();
                    CVector2D radarPosition;
                    CRadar::TransformRealWorldPointToRadarSpace(radarPosition, pedPosition);
                    if (IsPointInsideRadar(radarPosition)) {
                        CRadar::TransformRadarPointToScreenSpace(pedPosition, radarPosition);
                        float angle = pPed->GetHeading() - (CRadar::m_fRadarOrientation + 3.1415927);
                        CRGBA col = player->GetColorAsRGBA();
                        CRadar::DrawRotatingRadarSprite(&CRadar::RadarBlipSprites[RADAR_SPRITE_CENTRE], pedPosition.x, pedPosition.y, angle, RsGlobal.maximumWidth * 0.0015625 * 5.0, RsGlobal.maximumWidth * 0.0015625 * 5.0, CRGBA(col.r, col.g, col.b));

                    }
                }
            }
        }
        else
            continue;
    }
}

void nop_(PVOID address, int bytes) {
    DWORD NewProtection;
    VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &NewProtection);
    memset(address, 0x90, bytes);
    VirtualProtect(address, bytes, NewProtection, &NewProtection);
}

void CTimer__Update(const decltype(CTimerHook)& hook) {
    static bool sampInit{};
    if (!sampInit && samp::RefNetGame()) {
        nop_((void*)sampapi::GetAddress(0x13943), 8);
        CRadarHook.set_dest(0x588050);
        CRadarHook.set_cb(CRadar__DrawBlips);
        CRadarHook.install();
        sampInit = { true };
    }

    hook.get_trampoline()();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CTimerHook.set_dest(0x561B10);
        CTimerHook.set_cb(CTimer__Update);
        CTimerHook.install();
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}