#pragma once
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

// Request to write to kernel mode
#define IO_RETURNANTICHEATUSERMODE_PROCESSID_GMAE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x601, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _HIDE_VAD
{
	ULONGLONG base;             // Region base address
	ULONGLONG size;             // Region size
	ULONG pid;                  // Target process ID
} HIDE_VAD, *PHIDE_VAD;


typedef struct _KERNEL_READ_REQUEST
{
	ULONG UsermodeProgram;
	ULONG GameProcess;

} KERNEL_READ_REQUEST, *PKERNEL_READ_REQUEST;


typedef struct _KERNEL_HEARTBEAT_REQUEST
{
	ULONG Encrypt1;
	ULONG Encrypt2;
	ULONG Encrypt3;
	ULONG Encrypt4;
	ULONG Encrypt5;

} KERNEL_HEARTBEAT_REQUEST, *PKERNEL_HEARTBEAT_REQUEST;

typedef struct _KERNEL_THREAD_REQUEST
{
	ULONG ThreadID;
	ULONG ThreadID2;
	ULONG ThreadID3;
	ULONG ThreadID4;
	ULONG ThreadID5;
	ULONG ThreadID6;
	ULONG ThreadID7;
	ULONG ThreadID8;

} KERNEL_THREAD_REQUEST, *PKERNEL_THREAD_REQUEST;