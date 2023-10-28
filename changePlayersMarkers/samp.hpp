#ifndef _SAMP_LIBRARY_HPP_
#define _SAMP_LIBRARY_HPP_

#include <Windows.h>

namespace SAMP
{
	enum class sampVersion : int
	{
		unknown = 0,
		notLoaded = -1,
		R1 = 1,
		R3 = 3,
		R5 = 5,
		DL = 6
	}; // enum class sampVersion
	unsigned long GetSAMPHandle() {
		static unsigned long samp{ 0 };
		if (!samp) {
			samp = reinterpret_cast<unsigned long>(GetModuleHandleA("samp"));
			if (!samp || samp == -1 || reinterpret_cast<HANDLE>(samp) == INVALID_HANDLE_VALUE) {
				samp = 0;
			}
		}
		return samp;
	}
	enum sampVersion GetSAMPVersion() {
		static sampVersion sampVersion = sampVersion::unknown;
		if (sampVersion != sampVersion::unknown) {
			return sampVersion;
		}
		unsigned long samp = GetSAMPHandle();
		if (!samp) {
			sampVersion = sampVersion::notLoaded;
		}
		else {
			unsigned long EntryPoint = reinterpret_cast<IMAGE_NT_HEADERS*>(samp + reinterpret_cast<IMAGE_DOS_HEADER*>(samp)->e_lfanew)->OptionalHeader.AddressOfEntryPoint;
			switch (EntryPoint) {
			case (0x31DF13): {
				sampVersion = sampVersion::R1;
				break;
			}
			case (0xCC4D0): {
				sampVersion = sampVersion::R3;
				break;
			}
			case (0xCBC90): {
				sampVersion = sampVersion::R5;
				break;
			}
			case (0xFDB60): {
				sampVersion = sampVersion::DL;
				break;
			}
			default: {
				sampVersion = sampVersion::unknown;
				break;
			}
			}
		}
		return sampVersion;
	}
	bool IsSAMPInitialized() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0F8) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8DC) != nullptr;
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB94) != nullptr;
		else if (GetSAMPVersion() == sampVersion::DL)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x2ACA24) != nullptr;
		return false;
	}

	void* RefNetGame() {
		if (GetSAMPVersion() == sampVersion::R1)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x21A0F8);
		else if (GetSAMPVersion() == sampVersion::R3)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26E8DC);
		else if (GetSAMPVersion() == sampVersion::R5)
			return *reinterpret_cast<void**>(GetSAMPHandle() + 0x26EB94);
		return nullptr;
	}

	void* CNetGame__GetPlayerPool() {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<void*(__thiscall*)(void*)>(GetSAMPHandle() + 0x1160)(RefNetGame());
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<void* (__thiscall*)(void*)>(GetSAMPHandle() + 0x1160)(RefNetGame());
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<void* (__thiscall*)(void*)>(GetSAMPHandle() + 0x1170)(RefNetGame());
		return nullptr;
	}

	void* CPlayerPool__GetPlayer(unsigned short nId) {
		if (GetSAMPVersion() != sampVersion::unknown && GetSAMPVersion() != sampVersion::notLoaded)
			return reinterpret_cast<void*(__thiscall*)(void*, unsigned short)>(GetSAMPHandle() + 0x10F0)(CNetGame__GetPlayerPool(), nId);
		else
			return false;

	}

	unsigned long CRemotePlayer__GetColorAsRGBA(void* pRemotePlayer) {
		if (GetSAMPVersion() == sampVersion::R1)
			return reinterpret_cast<unsigned long (__thiscall*)(void*)>(GetSAMPHandle() + 0x129F0)(pRemotePlayer);
		else if (GetSAMPVersion() == sampVersion::R3)
			return reinterpret_cast<unsigned long (__thiscall*)(void*)>(GetSAMPHandle() + 0x15C00)(pRemotePlayer);
		else if (GetSAMPVersion() == sampVersion::R5)
			return reinterpret_cast<unsigned long (__thiscall*)(void*)>(GetSAMPHandle() + 0x16170)(pRemotePlayer);
		return 0;
	}

	bool CRemotePlayer__DoesExist(void* pRemotePlayer) {
		if (GetSAMPVersion() != sampVersion::unknown && GetSAMPVersion() != sampVersion::notLoaded)
			return reinterpret_cast<bool(__thiscall*)(void*)>(GetSAMPHandle() + 0x1080)(pRemotePlayer);
		else
			return false;
	}

	bool CPlayerPool__IsConnected(unsigned short nId) {
		if (GetSAMPVersion() != sampVersion::unknown && GetSAMPVersion() != sampVersion::notLoaded)
			return reinterpret_cast<bool(__thiscall*)(void*, unsigned short)>(GetSAMPHandle() + 0x10B0)(CNetGame__GetPlayerPool(), nId);
		else
			return false;
	}

	CPed* GetGamePed(void* pRemotePlayer) {
		if (GetSAMPVersion() == sampVersion::R1) {
			uint32_t sampPed = *(uint32_t*)((uint32_t)pRemotePlayer + 0x00);
			return *(CPed**)(sampPed + 0x2A4);
		}
		else if (GetSAMPVersion() == sampVersion::R3) {
			uint32_t sampPed = *(uint32_t*)((uint32_t)pRemotePlayer + 0x00);
			return *(CPed**)(sampPed + 0x2A4);
		}
		else if (GetSAMPVersion() == sampVersion::R5) {
			uint32_t sampPed = *(uint32_t*)((uint32_t)pRemotePlayer + 0x1DD);
			return *(CPed**)(sampPed + 0x2A4);
		}
		return 0;
	}
}; // namespace SAMP

#endif // !_SAMP_LIBRARY_HPP_