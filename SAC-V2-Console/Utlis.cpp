#include "Utlis.h"

namespace Utlis
{
	DWORD Injection(PCWSTR pszLibFile, DWORD dwProcessId)
	{
		// Calculate the number of bytes needed for the DLL's pathname
		DWORD dwSize = (lstrlenW(pszLibFile) + 1) * sizeof(wchar_t);

		// Get process handle passing in the process ID
		HANDLE hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |
			PROCESS_CREATE_THREAD |
			PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE,
			FALSE, dwProcessId);
		if (hProcess == NULL)
		{
			//wprintf(TEXT("[-] Error: Could not open process for PID (%d).\n"), dwProcessId);
			return(1);
		}

		// Allocate space in the remote process for the pathname
		LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL)
		{
			//wprintf(TEXT("[-] Error: Could not allocate memory inside PID (%d).\n"), dwProcessId);
			return(1);
		}

		// Copy the DLL's pathname to the remote process address space
		DWORD n = WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)pszLibFile, dwSize, NULL);
		if (n == 0)
		{
			//wprintf(TEXT("[-] Error: Could not write any bytes into the PID [%d] address space.\n"), dwProcessId);
			return(1);
		}

		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL)
		{
			//wprintf(TEXT("[-] Error: Could not find LoadLibraryA function inside kernel32.dll library.\n"));
			return(1);
		}

		// Create a remote thread that calls LoadLibraryW(DLLPathname)
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (hThread == NULL)
		{
			//wprintf(TEXT("[-] Error: Could not create the Remote Thread.\n"));
			return(1);
		}
		else
		{

		}
			//wprintf(TEXT("[+] Success: DLL injected via CreateRemoteThread().\n"));

		// Wait for the remote thread to terminate
		WaitForSingleObject(hThread, INFINITE);

		// Free the remote memory that contained the DLL's pathname and close Handles
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != NULL)
			CloseHandle(hThread);

		if (hProcess != NULL)
			CloseHandle(hProcess);

	}

	BOOL IsRunAsAdministrator()
	{
		BOOL fIsRunAsAdmin = FALSE;
		DWORD dwError = ERROR_SUCCESS;
		PSID pAdministratorsGroup = NULL;

		SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
		if (!AllocateAndInitializeSid(
			&NtAuthority,
			2,
			SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0,
			&pAdministratorsGroup))
		{
			dwError = GetLastError();
			goto Cleanup;
		}

		if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
		{
			dwError = GetLastError();
			goto Cleanup;
		}

	Cleanup:

		if (pAdministratorsGroup)
		{
			FreeSid(pAdministratorsGroup);
			pAdministratorsGroup = NULL;
		}

		if (ERROR_SUCCESS != dwError)
		{
			throw dwError;
		}
		return fIsRunAsAdmin;
	}

	BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
	{
		using namespace std;
		OverlayFinderParams& params = *(OverlayFinderParams*)lParam;

		unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

		// If looking for windows of a specific PDI
		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		if (params.pidOwner != NULL)
			if (params.pidOwner == pid)
				++satisfiedCriteria; // Doesn't belong to the process targeted
			else
				++unSatisfiedCriteria;

		// If looking for windows of a specific class
		wchar_t className[MAX_CLASSNAME] = L"";
		GetClassName(hwnd, (LPSTR)className, MAX_CLASSNAME);
		wstring classNameWstr = className;
		if (params.wndClassName != L"")
			if (params.wndClassName == classNameWstr)
				++satisfiedCriteria; // Not the class targeted
			else
				++unSatisfiedCriteria;

		// If looking for windows with a specific name
		wchar_t windowName[MAX_WNDNAME] = L"";
		GetWindowText(hwnd, (LPSTR)windowName, MAX_CLASSNAME);
		wstring windowNameWstr = windowName;
		if (params.wndName != L"")
			if (params.wndName == windowNameWstr)
				++satisfiedCriteria; // Not the class targeted
			else
				++unSatisfiedCriteria;

		// If looking for window at a specific position
		RECT pos;
		GetWindowRect(hwnd, &pos);
		if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
			if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		// If looking for window of a specific size
		POINT res = { pos.right - pos.left, pos.bottom - pos.top };
		if (params.res.x || params.res.y)
			if (res.x == params.res.x && res.y == params.res.y)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		// If looking for windows taking more than a specific percentage of all the screens
		float ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
		float ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
		float percentAllScreens = ratioAllScreensX * ratioAllScreensY * 100;
		if (params.percentAllScreens != 0.0f)
			if (percentAllScreens >= params.percentAllScreens)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		// If looking for windows taking more than a specific percentage or the main screen
		RECT desktopRect;
		GetWindowRect(GetDesktopWindow(), &desktopRect);
		POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
		float ratioMainScreenX = res.x / desktopRes.x;
		float ratioMainScreenY = res.y / desktopRes.y;
		float percentMainScreen = ratioMainScreenX * ratioMainScreenY * 100;
		if (params.percentMainScreen != 0.0f)
			if (percentAllScreens >= params.percentMainScreen)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		// Looking for windows with specific styles
		LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
		if (params.style)
			if (params.style & style)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		// Looking for windows with specific extended styles
		LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		if (params.styleEx)
			if (params.styleEx & styleEx)
				++satisfiedCriteria;
			else
				++unSatisfiedCriteria;

		if (!satisfiedCriteria)
			return TRUE;

		if (params.satisfyAllCriteria && unSatisfiedCriteria)
			return TRUE;

		// If looking for multiple windows
		params.hwnds.push_back(hwnd);
		return TRUE;
	}

	std::vector<HWND> OverlayFinder(OverlayFinderParams params)
	{
		EnumWindows(EnumWindowsCallback, (LPARAM)&params);
		return params.hwnds;
	}
}
