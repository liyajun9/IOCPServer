#include "pch.h"
#include "SystemException.h"
#include <sstream>

#pragma warning(disable:4996)

YSystemException::YSystemException(const char* file/* = "<unknown file>"*/, const char* func /*= "<unknown func>"*/, int line /*= -1*/, const char* type /*= "YSystemException"*/) noexcept
	:YException("", file, func, line, type)
	,dwErrCode(GetLastError())
{
	sMessage = TranslateErrCode(dwErrCode);
}

const char* YSystemException::what() const noexcept
{
	if (sWhat.empty())
		toString();
	return sWhat.c_str();
}

std::string YSystemException::toString() const
{
	if(sWhat.empty()){
		std::stringstream sstr;
		sstr << sType << " caught: (" << dwErrCode << ")" <<sMessage << ".\tat " << sFile << "." << sFunc << " line " << nLine;
		sWhat = sstr.str();
	}
	return sWhat;
}

std::string YSystemException::TranslateErrCode(DWORD errCode)
{
	LPVOID lpMsgBuf;
	if (!FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	)) {
		return TranslateErrCode(GetLastError());
	}
	std::string sRet = (LPSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return sRet;
}

