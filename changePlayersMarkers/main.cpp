#include "plugin.h"
#include "CRadar.h"
#include "CTxdStore.h"
#include "samp.hpp"

using namespace plugin;

CSprite2d sprite;

void nop_(PVOID address, int bytes) {
    DWORD NewProtection;
    VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &NewProtection);
    memset(address, 0x90, bytes);
    VirtualProtect(address, bytes, NewProtection, &NewProtection);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Events::initGameEvent += [] {
            int ptr = CTxdStore::FindTxdSlot("hud");
            CTxdStore::SetCurrentTxd(ptr);
            sprite.SetTexture("playersIcon");
            CTxdStore::PopCurrentTxd();

        };

        Events::drawBlipsEvent += [] {
            static bool init{};
            if (SAMP::IsSAMPInitialized()) {
                if (!init) {
                    switch (SAMP::GetSAMPVersion())
                    {
                    case SAMP::sampVersion::R1:
                        nop_((void*)(SAMP::GetSAMPHandle() + 0x13943), 8);
                        break;
                    case SAMP::sampVersion::R3:
                        nop_((void*)(SAMP::GetSAMPHandle() + 0x16B40), 8);
                        break;
                    case SAMP::sampVersion::R5:
                        nop_((void*)(SAMP::GetSAMPHandle() + 0x171EE), 8);
                        break;
                    default:
                        break;
                    }
                    init = true;
                }
                for (int i = 0; i < 1004; i++) {
                    void* pool = SAMP::CNetGame__GetPlayerPool();
                    if (SAMP::CPlayerPool__IsConnected(i)) {
                        void* player = SAMP::CPlayerPool__GetPlayer(i);
                        if (SAMP::CRemotePlayer__DoesExist(player)) {
                            CPed* pPed = SAMP::GetGamePed(player);
                            if (pPed != nullptr) {
                                CVector2D pedPosition = pPed->GetPosition();
                                CVector2D radarPosition;
                                CRadar::TransformRealWorldPointToRadarSpace(radarPosition, pedPosition);
                                if (IsPointInsideRadar(radarPosition)) {
                                    CRadar::TransformRadarPointToScreenSpace(pedPosition, radarPosition);
                                    float angle = pPed->GetHeading() - (CRadar::m_fRadarOrientation + 3.1415927);
                                    CRGBA col = SAMP::CRemotePlayer__GetColorAsRGBA(player);
                                    CRadar::DrawRotatingRadarSprite(sprite.m_pTexture != nullptr ? &sprite : &CRadar::RadarBlipSprites[RADAR_SPRITE_CENTRE],
                                        pedPosition.x, pedPosition.y, angle, RsGlobal.maximumWidth * 0.0015625 * 5.0,
                                        RsGlobal.maximumWidth * 0.0015625 * 5.0, CRGBA(col.r, col.g, col.b));
                                }
                            }
                        }
                    }
                    else
                        continue;
                }
            }
        };
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}