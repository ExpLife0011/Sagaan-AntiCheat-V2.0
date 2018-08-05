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

ULONG Formula1(ULONG x)
{

	ULONG y = x / 2;
	ULONG sum = ((x*(x - 1)) / 2) - ((y*(y + 1)) / 2);
	sum += x * y ^ (x + y) - x;
	return sum;
}
ULONG Formula2(ULONG x)
{

	ULONG y = x * 2;
	ULONG sum = ((x*(x - 1)) / 2) + ((y*(y + 1)) / 2);
	sum += x * y ^ (x + y) - y;
	return sum;
}
ULONG Formula3(ULONG x)
{
	ULONG y = x + x^x - x;
	ULONG sum = ((x*(x - 1)) / 2) - ((y*(y + 1)) / 2);
	sum += x * y ^ (x + y) - x;
	return sum;
}
ULONG Formula4(ULONG x)
{

	ULONG y = x + 2;
	ULONG sum = ((x*(x - 1)) / 2) * ((y*(y + 1)) / 2);
	sum += x * y ^ (x + y) - y;
	return sum;
}
ULONG Formula5(ULONG x)
{
	ULONG y = x * 2 + x;
	ULONG sum = ((x*(x - 1)) / 2) / ((y*(y + 1)) / 2);
	sum += x * y ^ (x + y) - x;
	return sum;
}