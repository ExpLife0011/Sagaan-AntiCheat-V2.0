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
#include <wintrust.h>
#include <softpub.h>
#include <mscat.h>

namespace WhiteListedDLLs
{
	extern BOOL VerifyEmbeddedSignatures(_In_ PCWSTR FileName, _In_ HANDLE FileHandle, _In_ bool UseStrongSigPolicy);
}