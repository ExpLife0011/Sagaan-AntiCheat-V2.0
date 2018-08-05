#include "WhiteListedDLL.h"

void PrintError(_In_ DWORD Status)
{
	wchar_t text_buffer[500] = { 0 }; //temporary buffer
	swprintf(text_buffer, _countof(text_buffer), L"Error: 0x%08x (%d)\n", Status, Status); // convert
	OutputDebugStringW(text_buffer);
}

namespace WhiteListedDLLs
{
	BOOL VerifyEmbeddedSignatures(_In_ PCWSTR FileName,
		_In_ HANDLE FileHandle,
		_In_ bool UseStrongSigPolicy)
	{
		DWORD Error = ERROR_SUCCESS;
		bool WintrustCalled = false;
		GUID GenericActionId = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		WINTRUST_DATA WintrustData = {};
		WINTRUST_FILE_INFO FileInfo = {};
		WINTRUST_SIGNATURE_SETTINGS SignatureSettings = {};
		CERT_STRONG_SIGN_PARA StrongSigPolicy = {};

		// Setup data structures for calling WinVerifyTrust
		WintrustData.cbStruct = sizeof(WINTRUST_DATA);
		WintrustData.dwStateAction = WTD_STATEACTION_VERIFY;
		WintrustData.dwUIChoice = WTD_UI_NONE;
		WintrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
		WintrustData.dwUnionChoice = WTD_CHOICE_FILE;

		FileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO_);
		FileInfo.hFile = FileHandle;
		FileInfo.pcwszFilePath = FileName;
		WintrustData.pFile = &FileInfo;

		//
		// First verify the primary signature (index 0) to determine how many secondary signatures
		// are present. We use WSS_VERIFY_SPECIFIC and dwIndex to do this, also setting 
		// WSS_GET_SECONDARY_SIG_COUNT to have the number of secondary signatures returned.
		//
		SignatureSettings.cbStruct = sizeof(WINTRUST_SIGNATURE_SETTINGS);
		SignatureSettings.dwFlags = WSS_GET_SECONDARY_SIG_COUNT | WSS_VERIFY_SPECIFIC;
		SignatureSettings.dwIndex = 0;
		WintrustData.pSignatureSettings = &SignatureSettings;

		if (UseStrongSigPolicy != false)
		{
			StrongSigPolicy.cbSize = sizeof(CERT_STRONG_SIGN_PARA);
			StrongSigPolicy.dwInfoChoice = CERT_STRONG_SIGN_OID_INFO_CHOICE;
			StrongSigPolicy.pszOID = szOID_CERT_STRONG_SIGN_OS_CURRENT;
			WintrustData.pSignatureSettings->pCryptoPolicy = &StrongSigPolicy;
		}

		Error = WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
		WintrustCalled = true;
		if (Error != ERROR_SUCCESS)
		{
			PrintError(Error);
			if (WintrustCalled != false)
			{
				WintrustData.dwStateAction = WTD_STATEACTION_CLOSE;
				WinVerifyTrust(NULL, &GenericActionId, &WintrustData);
			}

			return FALSE;
		}
		return TRUE;
	}
}