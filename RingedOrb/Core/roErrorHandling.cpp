#include "roErrorHandling.h"
#include <comdef.h>


	ROrbException::ROrbException(HRESULT hr, std::wstring fileName, std::wstring functionName, int lineNumber) 	{
		Hr = hr;
		FileName	= fileName;
		FuncName	= functionName;
		LineNumber	= lineNumber;
	}
	void ROrbException::DisplayError() {
		_com_error err(Hr);
		std::wstring ExptMessage = L"Error when calling " + FuncName + L" in " + FileName + L" at line " + 
			std::to_wstring(LineNumber) + L" " + err.ErrorMessage();
		MessageBox(0, ExptMessage.c_str(), L"Exception Thrown", MB_OK);
	}

	ROrbGenericException::ROrbGenericException(std::wstring filename, std::wstring message, int lineNumber)	{

		FileName	= filename;
		Message		= message;
		LineNumber	= lineNumber;
	}

	void ROrbGenericException::DisplayError() 	{
		std::wstring ExceptMessage = Message + L" " + FileName + L" at " + std::to_wstring(LineNumber);
		MessageBox(0, ExceptMessage.c_str(), L"Exception Thrown", MB_OK);
	}
