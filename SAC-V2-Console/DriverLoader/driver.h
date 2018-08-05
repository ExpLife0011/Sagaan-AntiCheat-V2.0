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
#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <windows.h>
#include <exception>

#define SVC_OK 0x01

/* ================================================
 * Begin CDriver_Loader Class Definition
* ================================================= */
class CDriver_Loader {
public:
	// Constructors
	CDriver_Loader(); // Default constructor
	CDriver_Loader(LPTSTR, LPTSTR, LPTSTR, DWORD); // Initalzing

	//	Destructor
	~CDriver_Loader();

	// Status functions
	inline bool isInit() const { return init; }
	inline bool isLoaded() const { return loaded; }
	inline bool isStarted() const { return started; }

	// Driver service functions
	DWORD InitSvc(LPTSTR filePath, LPTSTR serviceName, LPTSTR displayName, DWORD startType); // initalizing the driver service
	DWORD CreateSvc(); // Creating the driver service
	DWORD StartSvc(); // Starting the driver service
	DWORD StopSvc(); // Starting the driver service
	DWORD UnloadSvc(); // Unload the driver service 

private:

	LPTSTR mFilePath; //driver file path
	LPTSTR mServiceName; //service name
	LPTSTR mDisplayName; //dos service name

	DWORD mStartType; //start type

	SC_HANDLE mService; //service's handle

	// Status variables
	bool init; 
	bool loaded;
	bool started;	
};

#endif
