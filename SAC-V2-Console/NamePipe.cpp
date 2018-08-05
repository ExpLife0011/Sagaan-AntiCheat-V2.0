#include "NamePipe.h"

namespace NamedPipe
{
	HANDLE pipe;
	bool SendData(HANDLE pipe, BOOL result, const wchar_t *data)
	{

		DWORD numBytesWritten = 0;
		result = WriteFile(
			pipe, // handle to our outbound pipe
			data, // data to send
			wcslen(data) * sizeof(wchar_t), // length of data to send (bytes)
			&numBytesWritten, // will store actual amount of data sent
			NULL // not using overlapped IO
		);


		if (numBytesWritten) {
			return true;
		}
		else 
		{
			return false;
		}
	}

	bool CreatePipe()
	{
		pipe = CreateNamedPipe(
			"\\\\.\\pipe\\NamedPipeCommucationExternal", // name of the pipe
			PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
			PIPE_TYPE_BYTE, // send data as a byte stream
			1, // only allow 1 instance of this pipe
			0, // no outbound buffer
			0, // no inbound buffer
			0, // use default wait time
			NULL // use default security attributes
		);

		if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
			return false;
		}
	}
}
