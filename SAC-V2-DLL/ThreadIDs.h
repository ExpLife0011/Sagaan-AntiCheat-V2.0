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
#include <psapi.h>

namespace ThreadID
{
	extern DWORD TIDCheckUsermodePrcoess;
	extern DWORD TIDMain;
	extern DWORD TIDAntiDebug;
	extern DWORD TIDNamedPipe;
}