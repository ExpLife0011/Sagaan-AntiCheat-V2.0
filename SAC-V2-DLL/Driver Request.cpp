#include "Driver Request.h"

KERNEL_READ_REQUEST Blank{ 0 };
namespace DriverRequest
{
	KERNEL_READ_REQUEST CheckProcessIDs()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return Blank;

		DWORD Return, Bytes;
		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.GameProcess = GetCurrentProcessId();

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_RETURNANTICHEATUSERMODE_PROCESSID_GMAE, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return ReadRequest;
		}
		else
		{
			return ReadRequest;
		}
	}
}