#include <Windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <iterator>
#include <comdef.h>
#include <Wbemidl.h>
#include <VersionHelpers.h>
#include <atlstr.h>
#include <wbemidl.h>
#include <sstream>
#include <vector>

struct OverlayFinderParams {
	DWORD pidOwner = NULL;
	std::wstring wndClassName = L"";
	std::wstring wndName = L"";
	RECT pos = { 0, 0, 0, 0 }; // GetSystemMetrics with SM_CXSCREEN and SM_CYSCREEN can be useful here
	POINT res = { 0, 0 };
	float percentAllScreens = 0.0f;
	float percentMainScreen = 0.0f;
	DWORD style = NULL;
	DWORD styleEx = NULL;
	bool satisfyAllCriteria = false;
	std::vector<HWND> hwnds;
};

#define MAX_CLASSNAME 255
#define MAX_WNDNAME MAX_CLASSNAME

namespace Utlis
{
	extern BOOL IsRunAsAdministrator();
	extern DWORD Injection(PCWSTR pszLibFile, DWORD dwProcessId);

	// https://www.unknowncheats.me/forum/anti-cheat-bypass/263403-window-hijacking-dont-overlay-betray.html
	extern BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
	extern std::vector<HWND> OverlayFinder(OverlayFinderParams params);
}
