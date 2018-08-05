#pragma once
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <TlHelp32.h>

class CUtils
{
	public:
		static PVOID GetModuleAddressFromName(const wchar_t* c_wszName);
		static bool  IsLoadedAddress(DWORD dwAddress);

		static PVOID DetourFunc(BYTE *src, const BYTE *dst, const int len);
		static DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask);

		static bool  IsSuspendedThread(DWORD dwThreadId);

		static DWORD GetThreadOwnerProcessId(DWORD dwThreadID);
};

