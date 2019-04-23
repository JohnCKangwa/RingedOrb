#pragma once
#include "pch.h"
#include "..\Graphics\DX12IncludeFile.h"

using namespace Microsoft::WRL;


struct NoneCopyable {
	NoneCopyable() = default;
	NoneCopyable& operator= (const NoneCopyable& obj) = delete;
};

struct ROrbException : NoneCopyable {
	ROrbException(HRESULT hr, std::wstring fileName, std::wstring functionName, int lineNumber);
	void DisplayError();
private:
	HRESULT Hr;
	std::wstring FileName;
	std::wstring FuncName;
	int LineNumber;
};

struct ROrbGenericException : NoneCopyable {
	ROrbGenericException(std::wstring filename, std::wstring message, int lineNumber);
	void DisplayError();
private:
	std::wstring FileName;
	std::wstring Message;
	int LineNumber;
};

#ifndef HR
#define HR(x) \
{	\
	HRESULT hResult = (x);	\
	std::wstring filename = ConvertToMultiByteString(__FILE__);	\
	if(FAILED(hResult))	\
		throw ROrbException(x, filename, L#x, __LINE__); \
}
#endif

#ifndef THROW
#define THROW(x)	\
{ \
	std::wstring filename = ConvertToMultiByteString(__FILE__);	\
	std::wstring errmessage = /*L#*/x;	\
		throw ROrbGenericException(filename, errmessage, __LINE__ );	\
}
#endif
