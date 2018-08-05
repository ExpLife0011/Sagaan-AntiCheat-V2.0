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

namespace HeartbeatFormula
{
	extern ULONG Formula1(ULONG x);
	extern ULONG Formula2(ULONG x);
	extern ULONG Formula3(ULONG x);
	extern ULONG Formula4(ULONG x);
	extern ULONG Formula5(ULONG x);
}
