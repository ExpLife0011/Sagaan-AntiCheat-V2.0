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

#include "DriverIO.h"

extern HANDLE hDriver;
extern KERNEL_READ_REQUEST Blank;


namespace DriverRequest
{
	extern KERNEL_READ_REQUEST CheckProcessIDs();
}