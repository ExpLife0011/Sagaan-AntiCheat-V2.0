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
extern KERNEL_HEARTBEAT_REQUEST Blank; // For blank return
extern KERNEL_THREAD_REQUEST BlankThread; // For Blank return

namespace DriverRequest
{
	extern bool SendProtectedThreadID(KERNEL_THREAD_REQUEST Threads);
	extern bool SendCurrentProcessID();
	extern bool SendProcessIDs(ULONG GmaeProcess);
	extern bool TerminatePrcoess(ULONG PID);
	extern KERNEL_THREAD_REQUEST THREADPROTECTION_RETURN_Function();
	extern KERNEL_HEARTBEAT_REQUEST HEARTBEATCREATEPROCESS_RETURN_Function();
	extern bool HEARTBEATCREATEPROCESS_FORWARD_Function(ULONG Check_Encrypt_Random_Number1, ULONG Check_Encrypt_Random_Number2, ULONG Check_Encrypt_Random_Number3, ULONG Check_Encrypt_Random_Number4, ULONG Check_Encrypt_Random_Number5);
	extern bool HEARTBEATMAINSTART_FORWARD_Function(ULONG Encrypt_Random_Number1, ULONG Encrypt_Random_Number2, ULONG Encrypt_Random_Number3, ULONG Encrypt_Random_Number4, ULONG Encrypt_Random_Number5);
	extern KERNEL_HEARTBEAT_REQUEST HEARTBEATMAINSTART_RETURN_Function();
	extern bool HideVAD(DWORD pid, uint64_t base, uint32_t size);
}