#include "DLLInjectorDector.h"
#include "Utils.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <algorithm>
#include <TlHelp32.h>
#include "../WhiteListedDLL.h"
#include "../ErrorHandler.h"

typedef ULONG(NTAPI* RtlGetFullPathName_U)(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName);
static RtlGetFullPathName_U RtlGetFullPathName_U_ = nullptr;

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

std::string ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	char* buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
	std::string r(buf);
	delete[] buf;
	return r;
}

ULONG NTAPI RtlGetFullPathName_U_t(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName)
{
	//printf("RtlGetFullPathName_U_t -> %ls - %u\n", FileName, Size);

	auto pModuleBase = CUtils::GetModuleAddressFromName(FileName);
	if (pModuleBase)
	{
		wchar_t text_buffer[500] = { 0 }; //temporary buffer
		swprintf(text_buffer, _countof(text_buffer), L"File Name: %s File Buffer: %s File ShortName: %s", FileName, Buffer, ShortName); // convert
		OutputDebugStringW(text_buffer);
		//MessageBox(NULL, (LPCWSTR)pModuleBase, FileName, MB_OK);
		//ErrorHandler::ErrorMessage("19", 2);

	}

	return RtlGetFullPathName_U_(FileName, Size, Buffer, ShortName);
}

void InitializeDLLCheck()
{
	auto hNtdll = LoadLibraryA("ntdll.dll");
	if (hNtdll)
	{
		//printf("hNtdll: %p\n", hNtdll);
		assert(hNtdll);

		auto RtlGetFullPathName_U_o = reinterpret_cast<RtlGetFullPathName_U>(GetProcAddress(hNtdll, "RtlGetFullPathName_U"));
		//printf("RtlGetFullPathName_U: %p\n", RtlGetFullPathName_U_o);
		assert(RtlGetFullPathName_U_o);

		RtlGetFullPathName_U_ = reinterpret_cast<RtlGetFullPathName_U>(CUtils::DetourFunc(reinterpret_cast<PBYTE>(RtlGetFullPathName_U_o), reinterpret_cast<PBYTE>(RtlGetFullPathName_U_t), 5));
		//printf("RtlGetFullPathName_U(detour): %p\n", RtlGetFullPathName_U_);

		DWORD dwOld = 0;
		auto bProtectRet = VirtualProtect(RtlGetFullPathName_U_, 5, PAGE_EXECUTE_READWRITE, &dwOld);
		assert(bProtectRet);
	}
	else
	{
		ErrorHandler::ErrorMessage("0x4015 ( Please Restart )", 5);
		//ErrorHandler::ErrorMessage("18", 1);
	}
}