/* =========================================================
 * Driver.cpp												
 * *********************************************************
 *															
 * Class: CDriver_Loader									
 *															
 * Author:  Dan Revah										
 *															
 * Date:	03/12/2011 (DD/MM/YYYY)							
 * Version: 1.00											
 *															
 * (-!-) Kernel-mode driver loader							
 *	- Injecting into windows kernel							
 * ======================================================== */

#include "driver.h"
#include "..//AbortFailureDetects.h"

/* ===========================================================
 *  CDriver_Loader::CDriver_Loader()
 *
 *	(*) Default Constructor
 */
CDriver_Loader::CDriver_Loader(): 
	init(false), loaded(false), started(false), mFilePath(NULL), mServiceName(NULL), 
		mDisplayName(NULL), mStartType(0), mService(NULL)
{
}

/* ==========================================================================================================
 *  CDriver_Loader::CDriver_Loader(LPTSTR filePath, LPTSTR serviceName, LPTSTR displayName, DWORD startType)
 *
 *	(*) Initializing constructor
 */
CDriver_Loader::CDriver_Loader(LPTSTR filePath, LPTSTR serviceName, LPTSTR displayName, DWORD startType): 
	init(true), loaded(false), started(false), mFilePath(filePath), mServiceName(serviceName), 
		mDisplayName(displayName), mStartType(startType), mService(NULL)
{
}

/* ====================================
 *  CDriver_Loader::~CDriver_Loader()
 *
 *	(*) Destructor
 */
CDriver_Loader::~CDriver_Loader()
{
	UnloadSvc();

	mFilePath = NULL;
	mServiceName = NULL;
	mDisplayName = NULL;

	mStartType = 0;

	mService = NULL;
	
	init = false;
	loaded = false;
	started = false;

}

/* ===========================================================
 * DWORD CDriver_Loader::CreateSvc()
 *
 *	- Creating the driver service
 *  Return Value
 *	If the function succeeds, the return value is SVC_OK
 *  If the function failed it will throw a exception
 */
DWORD CDriver_Loader::CreateSvc()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		throw std::exception("OpenSCManager Failed with error code: "+GetLastError());

	mService = CreateService(hSCManager, mServiceName, mDisplayName, SC_MANAGER_ALL_ACCESS, 
			SERVICE_KERNEL_DRIVER, mStartType, SERVICE_ERROR_NORMAL, mFilePath, NULL, NULL, NULL, NULL, NULL );

	if (mService == NULL)
	{
		mService = OpenService(hSCManager, mServiceName, SERVICE_ALL_ACCESS);
		
		if (mService == NULL) 
		{
			CloseServiceHandle(hSCManager);
			throw std::exception("CreateService Failed with error code: "+GetLastError());
		}
	}
	
	loaded = true;
	CloseServiceHandle(hSCManager);
	
	return SVC_OK;
}


/* ============================================================================================================
 * DWORD CDriver_Loader::InitSvc(LPTSTR filePath, LPTSTR serviceName, LPTSTR displayName, DWORD startType)
 *
 *	- Initilazing the service parameters
 *
 *	Parameters:
 *  filePath - The fully-qualified path to the service binary file
 *	serviceName - The service name
 *  displayName - The dos-service name
 *	startType - The service start options
 *
 *  Return Value
 *	If the function succeeds or already initialzed, the return value is SV_OK
 */
DWORD CDriver_Loader::InitSvc(LPTSTR filePath, LPTSTR serviceName, LPTSTR displayName, DWORD startType)
{
	if (isInit())
		return SVC_OK;

	mFilePath = filePath;
	mServiceName = serviceName;
	mDisplayName = displayName;
	mStartType = startType;

	mService = NULL;

	init = true;
	loaded = false;
	started = false;

	return SVC_OK;
}

/* ==============================================================
 * SVC_Result CDriver_Loader::StartSvc()
 *
 *	- Initilazing the service parameters
 *
 *  Return Value
 *	If the function succeeds, the return value is SVC_OK
 *  If the function failed it will throw a exception
 */
DWORD CDriver_Loader::StartSvc()
{
	if (!isLoaded()) 
		throw std::exception("Service is not loaded");

	if (isStarted())
		return SVC_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		throw std::exception("OpenSCManager Failed with error code: "+GetLastError());

	mService = OpenService(hSCManager, mServiceName, SERVICE_ALL_ACCESS);
		
	if (mService == NULL) 
	{
		CloseServiceHandle(hSCManager);
		throw std::exception("OpenService Failed with error code: "+GetLastError());
	}

	if (StartService(mService,0,NULL)== NULL)
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(mService);
		throw std::exception("StartService Failed with error code: " + GetLastError());
	}

	CloseServiceHandle(hSCManager);
	started = true;

	return SVC_OK;
}

/* ==============================================================
 * DWORD CDriver_Loader::StopSvc()
 *
 *	- Initilazing the service parameters
 *
 *  Return Value
 *	If the function succeeds, the return value is SVC_OK
 *  If the function failed it will throw a exception
 */
DWORD CDriver_Loader::StopSvc()
{
	SERVICE_STATUS ss;

	if (!isStarted()) 
		return SVC_OK;

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		throw std::exception("OpenSCManager Failed with error code: "+GetLastError());

	mService = OpenService(hSCManager, mServiceName, SERVICE_ALL_ACCESS);
		
	if (mService == NULL) 
	{
		CloseServiceHandle(hSCManager);
		throw std::exception("OpenService Failed with error code: "+GetLastError());
	}

	if (ControlService(mService,SERVICE_CONTROL_STOP,&ss)== NULL)
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(mService);
		throw std::exception("ControlService Failed with error code: "+GetLastError());
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(mService);
	started = false;

	return SVC_OK;
}

/* ==============================================================
 * DWORD CDriver_Loader::UnloadSvc()
 *
 *	- Unloading the service
 *
 *  Return Value
 *	If the function succeeds, the return value is SVC_OK
 *  If the function failed it will throw a exception
 */
DWORD CDriver_Loader::UnloadSvc()
{
	if (!isLoaded())
		return SVC_OK;

	if (isStarted()) 
	{
		if (StopSvc() != SVC_OK)
			throw std::exception("Unloading driver Failed with error code: "+GetLastError());
	}

	SC_HANDLE hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);

	if (hSCManager == NULL)
		throw std::exception("OpenSCManager Failed with error code: "+GetLastError());

	mService = OpenService(hSCManager, mServiceName, SERVICE_ALL_ACCESS);
		
	if (mService == NULL) 
	{
		CloseServiceHandle(hSCManager);
		throw std::exception("OpenService Failed with error code: "+GetLastError());
	}

	DeleteService(mService);
	CloseServiceHandle(hSCManager);
	
	loaded = false;

	return SVC_OK;
}
