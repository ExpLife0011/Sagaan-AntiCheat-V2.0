#include "DriverIORequests.h"


KERNEL_HEARTBEAT_REQUEST Blank; // For blank return
KERNEL_THREAD_REQUEST BlankThread; // For Blank return

namespace DriverRequest
{
	bool SendProcessIDs(ULONG GmaeProcess)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.UsermodeProgram = GetCurrentProcessId();
		ReadRequest.GameProcess = GmaeProcess;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_SEND_PROCESSID, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SendProtectedThreadID(KERNEL_THREAD_REQUEST Threads)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;


		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_PROTECTIONT_THREADS, &Threads,
			sizeof(Threads), &Threads, sizeof(Threads), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool HideVAD(DWORD pid, uint64_t base, uint32_t size)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		HIDE_VAD ReadRequest;

		ReadRequest.base = base;
		ReadRequest.size = size;
		ReadRequest.pid = pid;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_VADPROTECTION, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TerminatePrcoess(ULONG PID)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.UsermodeProgram = PID;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_TerminateProcess, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SendCurrentProcessID()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.UsermodeProgram = (ULONG)GetCurrentProcessId();

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_SEND_CURRENTPROCESS, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	KERNEL_THREAD_REQUEST THREADPROTECTION_RETURN_Function()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return BlankThread;

		DWORD Return, Bytes;
		KERNEL_THREAD_REQUEST ReadRequest;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, THREADPROTECTION_HEARTBEATFUCNTION_RETURN, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return ReadRequest;
		}
		else
		{
			return ReadRequest;
		}
	}



	KERNEL_HEARTBEAT_REQUEST HEARTBEATCREATEPROCESS_RETURN_Function()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return Blank;

		DWORD Return, Bytes;
		KERNEL_HEARTBEAT_REQUEST ReadRequest;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, HEARTBEATCREATEPROCESS_RETURN, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return ReadRequest;
		}
		else
		{
			return ReadRequest;
		}
	}
	bool HEARTBEATCREATEPROCESS_FORWARD_Function(ULONG Check_Encrypt_Random_Number1, ULONG Check_Encrypt_Random_Number2, ULONG Check_Encrypt_Random_Number3, ULONG Check_Encrypt_Random_Number4, ULONG Check_Encrypt_Random_Number5)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		KERNEL_HEARTBEAT_REQUEST ReadRequest;

		ReadRequest.Encrypt1 = Check_Encrypt_Random_Number1;
		ReadRequest.Encrypt2 = Check_Encrypt_Random_Number2;
		ReadRequest.Encrypt3 = Check_Encrypt_Random_Number3;
		ReadRequest.Encrypt4 = Check_Encrypt_Random_Number4;
		ReadRequest.Encrypt5 = Check_Encrypt_Random_Number5;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, HEARTBEATCREATEPROCESS_FORWARD, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	bool HEARTBEATMAINSTART_FORWARD_Function(ULONG Encrypt_Random_Number1, ULONG Encrypt_Random_Number2, ULONG Encrypt_Random_Number3, ULONG Encrypt_Random_Number4, ULONG Encrypt_Random_Number5)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		DWORD Return, Bytes;
		KERNEL_HEARTBEAT_REQUEST ReadRequest;

		ReadRequest.Encrypt1 = Encrypt_Random_Number1;
		ReadRequest.Encrypt2 = Encrypt_Random_Number2;
		ReadRequest.Encrypt3 = Encrypt_Random_Number3;
		ReadRequest.Encrypt4 = Encrypt_Random_Number4;
		ReadRequest.Encrypt5 = Encrypt_Random_Number5;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, HEARTBEATMAINSTART_FORWARD, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), &Bytes, NULL))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	KERNEL_HEARTBEAT_REQUEST HEARTBEATMAINSTART_RETURN_Function()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return Blank;

		DWORD Return, Bytes;
		KERNEL_HEARTBEAT_REQUEST ReadRequest;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, HEARTBEATMAINSTART_RETURN, &ReadRequest,
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