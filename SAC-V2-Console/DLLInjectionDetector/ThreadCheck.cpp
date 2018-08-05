#include "DLLInjectorDector.h"
#include "Utils.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include "../AbortFailureDetects.h"

typedef void(*LdrInitializeThunk)(PCONTEXT NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);
static LdrInitializeThunk LdrInitializeThunk_ = nullptr;

#define NtCurrentProcess			((HANDLE)(LONG_PTR)-1)
#define NtCurrentThread				((HANDLE)(LONG_PTR)-2)

typedef NTSTATUS(WINAPI* lpNtQueryInformationThread)(HANDLE, LONG, PVOID, ULONG, PULONG);
void LdrInitializeThunk_t(PCONTEXT NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
	auto GetThreadStartAddress = [](HANDLE hThread) -> DWORD {
		auto NtQueryInformationThread = (lpNtQueryInformationThread)GetProcAddress(LoadLibraryA("ntdll"), "NtQueryInformationThread");
		assert(NtQueryInformationThread);

		DWORD dwCurrentThreadAddress = 0;
		NtQueryInformationThread(hThread, 9 /* ThreadQuerySetWin32StartAddress */, &dwCurrentThreadAddress, sizeof(dwCurrentThreadAddress), NULL);
		return dwCurrentThreadAddress;
	};

	auto dwStartAddress = GetThreadStartAddress(NtCurrentThread);
	//printf("[*] A thread attached to process! Start address: %p\n", (void*)dwStartAddress);

	auto dwThreadId = GetThreadId(NtCurrentThread);
	//printf("\t* Thread: %u - Suspended: %d\n", dwThreadId, CUtils::IsSuspendedThread(dwThreadId));
	if (CUtils::IsSuspendedThread(dwThreadId))
	{
		ErrorHandler::ErrorMessage("3902", 5);
	}

	MODULEINFO user32ModInfo = { 0 };
	if (GetModuleInformation(NtCurrentProcess, LoadLibraryA("user32"), &user32ModInfo, sizeof(user32ModInfo)))
	{
		DWORD dwUser32Low = (DWORD)user32ModInfo.lpBaseOfDll;
		DWORD dwUser32Hi = (DWORD)user32ModInfo.lpBaseOfDll + user32ModInfo.SizeOfImage;
		if (dwStartAddress >= dwUser32Low && dwStartAddress <= dwUser32Hi)
		{
			ErrorHandler::ErrorMessage("02", 5);
			//printf("# WARNING # dwStartAddress in User32.dll\n");
		}
	}

	if (dwStartAddress == (DWORD)LoadLibraryA)
	{
		ErrorHandler::ErrorMessage("03", 5);
		//printf("# WARNING # dwStartAddress == LoadLibraryA\n");
	}
	else if (dwStartAddress == (DWORD)LoadLibraryW)
	{
		ErrorHandler::ErrorMessage("04", 5);
		//printf("# WARNING # dwStartAddress == LoadLibraryW\n");
	}
	else if (dwStartAddress == (DWORD)LoadLibraryExA)
	{
		ErrorHandler::ErrorMessage("05", 5);
		//printf("# WARNING # dwStartAddress == LoadLibraryExA\n");
	}
	else if (dwStartAddress == (DWORD)LoadLibraryExW)
	{
		ErrorHandler::ErrorMessage("06", 5);
		//printf("# WARNING # dwStartAddress == LoadLibraryExW\n");
	}
	else if (dwStartAddress == (DWORD)GetProcAddress(LoadLibraryA("ntdll"), "RtlUserThreadStart"))
	{
		ErrorHandler::ErrorMessage("07", 5);
		//printf("# WARNING # dwStartAddress == RtlUserThreadStart\n");
	}
	else if (dwStartAddress == (DWORD)GetProcAddress(LoadLibraryA("ntdll"), "NtCreateThread"))
	{
		ErrorHandler::ErrorMessage("08", 5);
		//printf("# WARNING # dwStartAddress == NtCreateThread\n");
	}
	else if (dwStartAddress == (DWORD)GetProcAddress(LoadLibraryA("ntdll"), "NtCreateThreadEx"))
	{
		ErrorHandler::ErrorMessage("09", 5);
		//printf("# WARNING # dwStartAddress == NtCreateThreadEx\n");
	}
	else if (dwStartAddress == (DWORD)GetProcAddress(LoadLibraryA("ntdll"), "RtlCreateUserThread"))
	{
		ErrorHandler::ErrorMessage("10", 5);
		//printf("# WARNING # dwStartAddress == RtlCreateUserThread\n");
	}

	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (VirtualQuery((LPCVOID)dwStartAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.Type != MEM_IMAGE)
		{
			ErrorHandler::ErrorMessage("13", 5);
		}

		if (dwStartAddress == (DWORD)mbi.AllocationBase)
		{
			ErrorHandler::ErrorMessage("14", 5);
		}
	}

	if (CUtils::IsLoadedAddress(dwStartAddress))
	{
		ErrorHandler::ErrorMessage("15", 5);
	}

	if (CUtils::GetThreadOwnerProcessId(dwThreadId) != GetCurrentProcessId())
	{
		ErrorHandler::ErrorMessage("16", 5);
	}

	IMAGE_SECTION_HEADER * pCurrentSecHdr = (IMAGE_SECTION_HEADER*)dwStartAddress;
	if (pCurrentSecHdr)
	{
		BOOL IsMonitored =
			(pCurrentSecHdr->Characteristics & IMAGE_SCN_MEM_EXECUTE) && (pCurrentSecHdr->Characteristics & IMAGE_SCN_MEM_READ) &&
			(pCurrentSecHdr->Characteristics & IMAGE_SCN_CNT_CODE) && !(pCurrentSecHdr->Characteristics & IMAGE_SCN_MEM_DISCARDABLE);

		if (IsMonitored)
		{
			//printf("Thread MOnitored");
		}
	}

	return LdrInitializeThunk_(NormalContext, SystemArgument1, SystemArgument2);
}

void InitializeThreadCheck()
{
	auto hNtdll = LoadLibraryA("ntdll.dll");
	if (hNtdll)
	{
		//printf("hNtdll: %p\n", hNtdll);
		assert(hNtdll);

		auto LdrInitializeThunk_o = reinterpret_cast<LdrInitializeThunk>(GetProcAddress(hNtdll, "LdrInitializeThunk"));
		//printf("LdrInitializeThunk: %p\n", LdrInitializeThunk_o);
		assert(LdrInitializeThunk_o);

		LdrInitializeThunk_ = reinterpret_cast<LdrInitializeThunk>(CUtils::DetourFunc(reinterpret_cast<PBYTE>(LdrInitializeThunk_o), reinterpret_cast<PBYTE>(LdrInitializeThunk_t), 5));
		//printf("LdrInitializeThunk(detour): %p\n", LdrInitializeThunk_);

		DWORD dwOld = 0;
		auto bProtectRet = VirtualProtect(LdrInitializeThunk_, 5, PAGE_EXECUTE_READWRITE, &dwOld);
		assert(bProtectRet);
	}
	else
	{
		ErrorHandler::ErrorMessage("17", 1);
	}
}

