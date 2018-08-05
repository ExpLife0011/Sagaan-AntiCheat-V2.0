#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <string>
#include "../SAC-V2-Console/DriverLoader/driver.h"

extern 	CDriver_Loader* driver;


namespace ErrorHandler
{
	extern bool isProcessRunning(int pid);
	extern bool LoadDriver();
	extern bool UnloadDriver();
	extern void ErrorMessage(std::string ErrorCode, int ErrorType);
}
