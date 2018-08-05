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
#include <psapi.h>
#include <wintrust.h>
#include <softpub.h>
#include <mscat.h>

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define GET_BITS( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define GET_BYTE( x )		(GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

namespace WhiteListedDLLs
{
	extern const char* RawDataAntiCheatDLL;
	extern long GetFileSize(std::string filename);
	extern const char* RawDataAntiCheatDLLTest;
	extern const std::vector<std::pair<std::string, const char*>> signatures;

	// Credits To Wlan For These Three Function
	extern inline bool is_match(const uint8_t* addr, const uint8_t* pat, const uint8_t* msk);
	extern inline uint8_t* find_pattern(uint8_t* range_start, const uint32_t len, const char* pattern);
	extern 	inline void OpenBinaryFile(const std::string & file, std::vector<uint8_t>& data);


	extern BOOL VerifyEmbeddedSignatures(_In_ PCWSTR FileName, _In_ HANDLE FileHandle, _In_ bool UseStrongSigPolicy);
}