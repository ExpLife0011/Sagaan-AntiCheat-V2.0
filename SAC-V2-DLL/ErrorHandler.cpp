#include "ErrorHandler.h"
#include <wtsapi32.h>
#include "AntiDebug.h"
#include "NamePipe.h"
#include "Driver Request.h"

#include <string>

CDriver_Loader* driver;

BOOL load_driver(std::string TargetDriver, std::string TargetServiceName, std::string TargetServiceDesc)
{
	SC_HANDLE ServiceManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!ServiceManager) return FALSE;
	SC_HANDLE ServiceHandle = CreateService(ServiceManager, TargetServiceName.c_str(), TargetServiceDesc.c_str(), SERVICE_START | DELETE | SERVICE_STOP, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, TargetDriver.c_str(), NULL, NULL, NULL, NULL, NULL);
	if (!ServiceHandle)
	{
		ServiceHandle = OpenService(ServiceManager, TargetServiceName.c_str(), SERVICE_START | DELETE | SERVICE_STOP);
		if (!ServiceHandle) return FALSE;
	}
	if (!StartServiceA(ServiceHandle, NULL, NULL)) return FALSE;
	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceManager);
	return TRUE;
}

BOOL delete_service(std::string TargetServiceName)
{
	SERVICE_STATUS ServiceStatus;
	SC_HANDLE ServiceManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!ServiceManager) return FALSE;
	SC_HANDLE ServiceHandle = OpenService(ServiceManager, TargetServiceName.c_str(), SERVICE_STOP | DELETE);
	if (!ServiceHandle) return FALSE;
	if (!ControlService(ServiceHandle, SERVICE_CONTROL_STOP, &ServiceStatus)) return FALSE;
	if (!DeleteService(ServiceHandle)) return FALSE;
	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceManager);
	return TRUE;
}

VOID MessageBox_(LPCSTR Text, LPCSTR Title)
{
	DWORD response;

	WTSSendMessageA(WTS_CURRENT_SERVER_HANDLE,       // hServer
		WTSGetActiveConsoleSessionId(),  // ID for the console seesion (1)
		const_cast<LPSTR>(Title),        // MessageBox Caption
		strlen(Title),                   // 
		const_cast<LPSTR>(Text),         // MessageBox Text
		strlen(Text),                    // 
		MB_OK,                           // Buttons, etc
		10,                              // Timeout period in seconds
		&response,                       // What button was clicked (if bWait == TRUE)
		FALSE);                          // bWait - Blocks until user click
}

namespace ErrorHandler
{
	bool Loaded = false;

	DWORD WINAPI CleanUpThread()
	{
		UnloadDriver();
		AntiDebug::HideThread(GetCurrentThread());
		Sleep(400);
		exit(1);
	}

	bool isProcessRunning(int pid)
	{
		HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(pe);

		if (Process32First(pss, &pe))
		{
			do
			{
				// pe.szExeFile can also be useful  
				if (pe.th32ProcessID == pid)
					return true;
			} while (Process32Next(pss, &pe));
		}

		CloseHandle(pss);

		return false;
	}

	bool LoadDriver()
	{
		return load_driver("C:\\SAC-V2-Driver.sys", "SAC Driver", "Sagaan-Anti Cheat");
	}
	bool UnloadDriver()
	{
		return delete_service("SAC Driver");
	}
	void ErrorMessage(std::string ErrorCode, int ErrorType)
	{
		CloseHandle(NamedPipe::pipe);
		std::string ErrorMessage = "SAC-Scan\n\nSagaan Anti Cheat Failure (Contact Support )\nError Code:  " + ErrorCode;
		std::string ErrorTypeStr;
		switch (ErrorType)
		{
		case 1:
			ErrorTypeStr = "Start Up Failure";
			break;
		case 2:
			ErrorTypeStr = "Cheat / Hack Detected";
			break;
		case 3:
			ErrorTypeStr = "Unknown Error";
			break;
		case 4:
			ErrorTypeStr = "Blacklisted Files Found";
			break;
		case 5:
			ErrorTypeStr = "Unusual Activies";
			break;
		case 6:
			ErrorTypeStr = "Debugger Found / VM Found";
			break;

		}

		UnloadDriver();
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CleanUpThread, NULL, NULL, NULL);
		MessageBox_((LPCSTR)ErrorMessage.c_str(), (LPCSTR)ErrorTypeStr.c_str());
		Sleep(400);
		exit(1);
	}
}