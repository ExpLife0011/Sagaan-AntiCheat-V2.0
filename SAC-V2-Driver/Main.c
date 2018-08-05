#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <ntdddisk.h>
#include <scsi.h>
#include <ntddscsi.h>
#include <mountdev.h>
#include <mountmgr.h>
#include <stdio.h>
#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <wdf.h>
#include <ntdef.h>

#include "BlackBone\VadRoutines.h"
#include "BlackBone\Routines.h"
#include "DriverIO.h"
#include "Formula.h"

DRIVER_INITIALIZE DriverEntry;
#pragma alloc_text(INIT, DriverEntry)
#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000
#define SYSTEM_PROCESS_ID (HANDLE)4

PVOID ObHandle = NULL;

ULONG ProtectedProcess = 0;
ULONG UsermodeAntiCheat = 0;
ULONG ProtectionThreads[7];


VOID CreateThreadNotifyRoutine(
	IN HANDLE ProcessId,
	IN HANDLE ThreadId,
	IN BOOLEAN Create
);

// Terminating a process of your choice using the PID, usefull if the cheat is also using a driver to strip it's handles and therefore you can forcefully close it using the driver
NTSTATUS TerminateProcess(ULONG targetPid)
{
	if (targetPid == ProtectedProcess)
	{
		ProtectedProcess = 0;
	}
	if (targetPid == UsermodeAntiCheat)
	{
		UsermodeAntiCheat = 0;
	}

	NTSTATUS NtRet = ((NTSTATUS)0x00000000L);
	PEPROCESS PeProc = { 0 };
	NtRet = PsLookupProcessByProcessId(targetPid, &PeProc);
	if (NtRet != ((NTSTATUS)0x00000000L))
	{
		return NtRet;
	}
	HANDLE ProcessHandle;
	NtRet = ObOpenObjectByPointer(PeProc, NULL, NULL, 25, *PsProcessType, KernelMode, &ProcessHandle);
	if (NtRet != ((NTSTATUS)0x00000000L))
	{
		return NtRet;
	}
	ZwTerminateProcess(ProcessHandle, 0);
	ZwClose(ProcessHandle);
	return NtRet;
}

NTSTATUS DriverDispatchRoutine(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	PVOID buffer;
	NTSTATUS NtStatus = STATUS_SUCCESS;
	PIO_STACK_LOCATION pIo;
	pIo = IoGetCurrentIrpStackLocation(pIrp);
	pIrp->IoStatus.Information = 0;
	switch (pIo->MajorFunction)
	{
	case IRP_MJ_CREATE:
		NtStatus = STATUS_SUCCESS;
		break;
	case IRP_MJ_READ:
		NtStatus = STATUS_SUCCESS;
		break;
	case IRP_MJ_WRITE:
		break;
	case IRP_MJ_CLOSE:
		NtStatus = STATUS_SUCCESS;
		break;
	default:
		NtStatus = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return NtStatus;
}

// This will be called, if the driver is unloaded or just returns something
VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "UNLOADED \n");
	UNICODE_STRING SACSymbolName;
	RtlInitUnicodeString(&SACSymbolName, L"\\DosDevices\\SACDriver"); // Giving the driver a symbol
	if (ObHandle)
	{
		ObUnRegisterCallbacks(ObHandle);
		ObHandle = NULL;
	}
	PsRemoveCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);
	IoDeleteSymbolicLink(&SACSymbolName);
	IoDeleteDevice(pDriverObject->DeviceObject);
}


NTSTATUS Create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS Close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

extern   NTSTATUS PsLookupProcessByProcessId(
	HANDLE ProcessId,
	PEPROCESS *Process
);




typedef struct _OB_REG_CONTEXT {
	USHORT Version;
	UNICODE_STRING Altitude;
	USHORT ulIndex;
	OB_OPERATION_REGISTRATION *OperationRegistration;
} REG_CONTEXT, *PREG_CONTEXT;

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	if ((ULONG)PsGetCurrentProcessId() == UsermodeAntiCheat)
	{
		return OB_PREOP_SUCCESS;
	}
	if ((ULONG)PsGetCurrentProcessId() == ProtectedProcess)
	{
		return OB_PREOP_SUCCESS;
	}

	if (PsGetThreadProcessId(OperationInformation->Object) == UsermodeAntiCheat)
	{

		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
		else
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);


	}
	else if (PsGetThreadProcessId(OperationInformation->Object) == ProtectedProcess)
		{

			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
			else
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);


		}

	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation)
{
	UNREFERENCED_PARAMETER(RegistrationContext);
	if (UsermodeAntiCheat == 0)
		return OB_PREOP_SUCCESS;

	if (ProtectedProcess == 0)
		return OB_PREOP_SUCCESS;

	PEPROCESS ProtectedProcessPEPROCESS;
	PEPROCESS ProtectedUserModeACPEPROCESS;

	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object,
		CurrentProcess = PsGetCurrentProcess();

	ULONG ulProcessId = PsGetProcessId(OpenedProcess);

	PsLookupProcessByProcessId(ProtectedProcess, &ProtectedProcessPEPROCESS);
	PsLookupProcessByProcessId(ProtectedProcess, &ProtectedUserModeACPEPROCESS);

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	if (ProtectedProcess != 0)
	{
		if (PsGetProcessId((PEPROCESS)OperationInformation->Object) == ProtectedProcess)
		{

			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) // striping handle 
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE);
			}
			else
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE);
			}
			return OB_PREOP_SUCCESS;
		}
	}

	if (UsermodeAntiCheat != 0)
	{
		if (PsGetProcessId((PEPROCESS)OperationInformation->Object) == UsermodeAntiCheat)
		{

			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) // striping handle 
			{
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE);
			}
			else
			{
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE);
			}
			return OB_PREOP_SUCCESS;
		}
	}
	return OB_PREOP_SUCCESS;
}

VOID CreateThreadNotifyRoutine(
	IN HANDLE ProcessId,
	IN HANDLE ThreadId,
	IN BOOLEAN Create
)
{
	if (!Create)
	{
		if (UsermodeAntiCheat == ProcessId)
		{
			for (int x = 0; x > 8; x++)
			{
				if ((ULONG)ThreadId == ProtectionThreads[x])
				{
					
					TerminateProcess(ProtectedProcess);
					TerminateProcess(UsermodeAntiCheat);
				}
			}
			
		}

		if (ProtectedProcess == ProcessId)
		{

		}

	}
}


VOID EnableCallBack()
{
	NTSTATUS NtHandleCallback = STATUS_UNSUCCESSFUL;
	NTSTATUS NtThreadCallback = STATUS_UNSUCCESSFUL;

	OB_OPERATION_REGISTRATION OBOperationRegistration[2];
	OB_CALLBACK_REGISTRATION OBOCallbackRegistration;
	REG_CONTEXT regContext;
	UNICODE_STRING usAltitude;
	memset(&OBOperationRegistration, 0, sizeof(OB_OPERATION_REGISTRATION));
	memset(&OBOCallbackRegistration, 0, sizeof(OB_CALLBACK_REGISTRATION));
	memset(&regContext, 0, sizeof(REG_CONTEXT));
	regContext.ulIndex = 1;
	regContext.Version = 120;
	RtlInitUnicodeString(&usAltitude, L"1000");

	if ((USHORT)ObGetFilterVersion() == OB_FLT_REGISTRATION_VERSION)
	{
		//OBOperationRegistration.ObjectType = PsProcessType; // Use To Strip Handle Permissions For Threads PsThreadType
		//OBOperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		//OBOperationRegistration.PostOperation = PostCallBack; // Giving the function which happens after creating
		//OBOperationRegistration.PreOperation = PreCallback; // Giving the function which happens before creating

		OBOperationRegistration[1].ObjectType = PsProcessType;
		OBOperationRegistration[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		OBOperationRegistration[1].PreOperation = ProcessPreCallback;
		OBOperationRegistration[1].PostOperation = NULL;


		OBOperationRegistration[0].ObjectType = PsThreadType;
		OBOperationRegistration[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
		OBOperationRegistration[0].PreOperation =  ThreadPreCallback;
		OBOperationRegistration[0].PostOperation = NULL;

		OBOCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
		OBOCallbackRegistration.OperationRegistrationCount = 2;
		OBOCallbackRegistration.RegistrationContext = NULL;
		OBOCallbackRegistration.OperationRegistration = &OBOperationRegistration;

		NtHandleCallback = ObRegisterCallbacks(&OBOCallbackRegistration, &ObHandle); // Register The CallBack
		PsSetCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);

		if (!NT_SUCCESS(NtHandleCallback))
		{
			if (ObHandle)
			{
				ObUnRegisterCallbacks(ObHandle);
				ObHandle = NULL;
			}
			DbgPrintEx(0, 0, "Error: ObRegisterCallbacks Has Failed\n");
		}
		else
		{
			
		}
	}
}

static ULONG KsecRandomSeed = 0x62b409a1;

NTSTATUS
NTAPI
KsecGenRandom(
	PVOID Buffer,
	SIZE_T Length)
{
	LARGE_INTEGER TickCount;
	ULONG i, RandomValue;
	PULONG P;

	/* Try to generate a more random seed */
	KeQueryTickCount(&TickCount);
	KsecRandomSeed ^= _rotl(TickCount.LowPart, (KsecRandomSeed % 23));

	P = Buffer;
	for (i = 0; i < Length / sizeof(ULONG); i++)
	{
		P[i] = RtlRandomEx(&KsecRandomSeed);
	}

	Length &= (sizeof(ULONG) - 1);
	if (Length > 0)
	{
		RandomValue = RtlRandomEx(&KsecRandomSeed);
		RtlCopyMemory(&P[i], &RandomValue, Length);
	}

	return STATUS_SUCCESS;
}

ULONG FOR1 = 100, FOR2 = 200, FOR3 = 300, FOR4 = 400, FOR5 = 500;
ULONG Encryption_Forward1 = 1, Encryption_Forward2 = 1, Encryption_Forward3 = 1, Encryption_Forward4 = 1, Encryption_Forward5 = 1;

BOOL Request_1 = FALSE;

BOOLEAN ProtectedGameRecieved = FALSE;
BOOLEAN UsermodeAntiCheatRecieved = FALSE;
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{


	NTSTATUS Status;
	ULONG BytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	// Code received from user space
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_SEND_CURRENTPROCESS && UsermodeAntiCheat < 1)
	{
		PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		UsermodeAntiCheat = ReadInput->UsermodeProgram;

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_READ_REQUEST);
	}


	if ((ULONG)UsermodeAntiCheat == (ULONG)PsGetProcessId(PsGetCurrentProcess()))
	{

		if (ControlCode == IO_SEND_PROCESSID && ProtectedGameRecieved == FALSE)
		{

			PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
			ProtectedProcess = ReadInput->GameProcess;

			if (ProtectedProcess > 0)
			{
				ProtectedGameRecieved = TRUE;
			}

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_READ_REQUEST);

		}

		else if (ControlCode == IO_PROTECTIONT_THREADS)
		{
			PKERNEL_THREAD_REQUEST ReadInput = (PKERNEL_THREAD_REQUEST)Irp->AssociatedIrp.SystemBuffer;
			ProtectionThreads[0] = ReadInput->ThreadID;
			ProtectionThreads[1] = ReadInput->ThreadID2;
			ProtectionThreads[2] = ReadInput->ThreadID3;
			ProtectionThreads[3] = ReadInput->ThreadID4;
			ProtectionThreads[4] = ReadInput->ThreadID5;
			ProtectionThreads[5] = ReadInput->ThreadID6;
			ProtectionThreads[6] = ReadInput->ThreadID7;
			ProtectionThreads[7] = ReadInput->ThreadID8;

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_THREAD_REQUEST);
		}
		else if (ControlCode == IO_TerminateProcess)
		{
			PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;

			TerminateProcess(ReadInput->UsermodeProgram);

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_READ_REQUEST);

		}
		else if (ControlCode == HEARTBEATMAINSTART_FORWARD)
		{


			PKERNEL_HEARTBEAT_REQUEST ReadInput = (PKERNEL_HEARTBEAT_REQUEST)Irp->AssociatedIrp.SystemBuffer;


			Encryption_Forward1 = ReadInput->Encrypt1;
			Encryption_Forward2 = ReadInput->Encrypt2;
			Encryption_Forward3 = ReadInput->Encrypt3;
			Encryption_Forward4 = ReadInput->Encrypt4;
			Encryption_Forward5 = ReadInput->Encrypt5;



			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_HEARTBEAT_REQUEST);


		}
		else if (ControlCode == HEARTBEATMAINSTART_RETURN)
		{


			PKERNEL_HEARTBEAT_REQUEST ReadInput = (PKERNEL_HEARTBEAT_REQUEST)Irp->AssociatedIrp.SystemBuffer;


			ReadInput->Encrypt1 = Formula1(Encryption_Forward1);
			ReadInput->Encrypt2 = Formula2(Encryption_Forward2);
			ReadInput->Encrypt3 = Formula3(Encryption_Forward3);
			ReadInput->Encrypt4 = Formula4(Encryption_Forward4);
			ReadInput->Encrypt5 = Formula5(Encryption_Forward5);

			//DbgPrintEx(0, 0, "HEARTBEATMAINSTART_RETURN Called: Formula1(Encryption_Forward1) %d", Formula1(Encryption_Forward1));

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_HEARTBEAT_REQUEST);


		}
		else if (ControlCode == HEARTBEATCREATEPROCESS_RETURN)
		{
			PKERNEL_HEARTBEAT_REQUEST ReadInput = (PKERNEL_HEARTBEAT_REQUEST)Irp->AssociatedIrp.SystemBuffer;
			KsecGenRandom(FOR1, sizeof(FOR1));
			KsecGenRandom(FOR2, sizeof(FOR2));
			KsecGenRandom(FOR3, sizeof(FOR3));
			KsecGenRandom(FOR4, sizeof(FOR4));
			KsecGenRandom(FOR5, sizeof(FOR5));


			ReadInput->Encrypt1 = FOR1;
			ReadInput->Encrypt2 = FOR2;
			ReadInput->Encrypt3 = FOR3;
			ReadInput->Encrypt4 = FOR4;
			ReadInput->Encrypt5 = FOR5;

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_HEARTBEAT_REQUEST);


		}
		else if (ControlCode == IO_VADPROTECTION)
		{
			PHIDE_VAD ReadInput = (PHIDE_VAD)Irp->AssociatedIrp.SystemBuffer;
			if (ReadInput->pid == ProtectedProcess || ReadInput == UsermodeAntiCheat)
			{
				// Enable it if you like. I dont need it so eh
				//BBHideVAD(ReadInput);
			}

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(HIDE_VAD);
		}
		else if (ControlCode == HEARTBEATCREATEPROCESS_FORWARD)
		{
			PKERNEL_HEARTBEAT_REQUEST ReadInput = (PKERNEL_HEARTBEAT_REQUEST)Irp->AssociatedIrp.SystemBuffer;

			if (ReadInput->Encrypt1 == Formula1(FOR1))
			{
				if (ReadInput->Encrypt2 == Formula2(FOR2))
				{
					if (ReadInput->Encrypt3 == Formula3(FOR3))
					{
						if (ReadInput->Encrypt4 == Formula4(FOR4))
						{
							if (ReadInput->Encrypt5 == Formula5(FOR5))
							{
								//DbgPrintEx(0, 0, "Process Id ( IOCONTROL )%s\n", (ULONG)PsGetProcessId(IoGetCurrentProcess()));
								Status = STATUS_SUCCESS;
								// GOOD
							}
							else
							{
								if (ProtectedProcess)
								{
									TerminateProcess(ProtectedProcess);
								}
								TerminateProcess(UsermodeAntiCheat);
								Status = STATUS_INVALID_PARAMETER;
								//BAD
								// ERROR
								// TERMINATE GAME OR PROTECTION 
								// BLUE SCREEN OF DEATH OR WHATEVER YOU LIKE
								DbgPrintEx(0, 0, "Error: ReadInput->Encrypt5 == Formula5(FOR5) Encrypt: %s Formula: %s \n", ReadInput->Encrypt5, Formula5(FOR5));
							}
						}
						else
						{
							if (ProtectedProcess)
							{

								TerminateProcess(ProtectedProcess);
							}
							TerminateProcess(UsermodeAntiCheat);
							Status = STATUS_INVALID_PARAMETER;


							//BAD
							// ERROR
							// TERMINATE GAME OR PROTECTION 
							// BLUE SCREEN OF DEATH OR WHATEVER YOU LIKE
							DbgPrintEx(0, 0, "Error: ReadInput->Encrypt4 == Formula4(FOR4) Encrypt: %s Formula: %s \n", ReadInput->Encrypt4, Formula4(FOR4));
						}
					}
					else
					{
						if (ProtectedProcess)
						{
							TerminateProcess(ProtectedProcess);
						}
						TerminateProcess(UsermodeAntiCheat);
						Status = STATUS_INVALID_PARAMETER;
						//BAD
						// ERROR
						// TERMINATE GAME OR PROTECTION 
						// BLUE SCREEN OF DEATH OR WHATEVER YOU LIKE
						DbgPrintEx(0, 0, "Error: ReadInput->Encrypt3 == Formula3(FOR3) Encrypt: %s Formula: %s \n", ReadInput->Encrypt3, Formula3(FOR3));
					}
				}
				else
				{
					if (ProtectedProcess)
					{
						TerminateProcess(ProtectedProcess);
					}
					TerminateProcess(UsermodeAntiCheat);
					Status = STATUS_INVALID_PARAMETER;
					//BAD
					// ERROR
					// TERMINATE GAME OR PROTECTION 
					// BLUE SCREEN OF DEATH OR WHATEVER YOU LIKE
					DbgPrintEx(0, 0, "Error: ReadInput->Encrypt2 == Formula2(FOR2) Encrypt: %s Formula: %s \n", ReadInput->Encrypt2, Formula2(FOR2));
				}
			}
			else
			{
				if (ProtectedProcess)
				{
					TerminateProcess(ProtectedProcess);
				}
				TerminateProcess(UsermodeAntiCheat);
				Status = STATUS_INVALID_PARAMETER;
				//BAD
				// ERROR
				// TERMINATE GAME OR PROTECTION 
				// BLUE SCREEN OF DEATH OR WHATEVER YOU LIKE
				DbgPrintEx(0, 0, "Error: ReadInput->Encrypt1 == Formula1(FOR1) Encrypt: %s Formula: %s \n", ReadInput->Encrypt1, Formula1(FOR1));
			}

			BytesIO = sizeof(KERNEL_HEARTBEAT_REQUEST);

		}

	}
	else if ((ULONG)ProtectedProcess == (ULONG)PsGetProcessId(PsGetCurrentProcess()))
	{
		if (ControlCode == IO_RETURNANTICHEATUSERMODE_PROCESSID_GMAE)
		{
			PKERNEL_READ_REQUEST ReadInput = (PKERNEL_READ_REQUEST)Irp->AssociatedIrp.SystemBuffer;
			ReadInput->UsermodeProgram = UsermodeAntiCheat;


			Status = STATUS_SUCCESS;
			BytesIO = sizeof(KERNEL_READ_REQUEST);

		}
	}

	// Complete the request
	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}

// Driver's Main function. This will be called and looped through till returned, or unloaded.
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pUniStr)
{
	DbgPrintEx(0, 0, "LOADED \n");
	UNICODE_STRING SACDriverName, SACSymbolName;
	NTSTATUS NtRet = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObj;
	RtlInitUnicodeString(&SACDriverName, L"\\Device\\SACDriver"); // Giving the driver a name
	RtlInitUnicodeString(&SACSymbolName, L"\\DosDevices\\SACDriver"); // Giving the driver a symbol
	UNICODE_STRING deviceNameUnicodeString, deviceSymLinkUnicodeString;
	NTSTATUS NtRet2 = IoCreateDevice(pDriverObject, 0, &SACDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj);

	IoCreateSymbolicLink(&SACSymbolName, &SACDriverName);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObj->Flags |= DO_DIRECT_IO;
	pDeviceObj->Flags &= (~DO_DEVICE_INITIALIZING);

	pDriverObject->DriverUnload = DriverUnload;

	EnableCallBack();

	return NtRet;

}
