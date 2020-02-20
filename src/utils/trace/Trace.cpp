#include "pch.h"
#include "Trace.h"

void NS_Yutils::_trace(const char *format, ...)
{
	char buffer[TRACE_BUFFER_LEN];
	memset(buffer, 0, TRACE_BUFFER_LEN * sizeof(char));

	time_t t = time(NULL);
	struct tm tmCur;
	localtime_s(&tmCur, &t); 
	sprintf_s(buffer,TRACE_BUFFER_LEN, "%04d-%02d-%02d %02d:%02d:%02d ", 
		tmCur.tm_year + 1900, 
		tmCur.tm_mon + 1,
		tmCur.tm_mday,
		tmCur.tm_hour,
		tmCur.tm_min,
		tmCur.tm_sec);

	va_list argptr;
	va_start(argptr, format);
	wvsprintfA(buffer + DATETIME_LEN, format, argptr);
	va_end(argptr);			

	OutputDebugStringA(buffer);
	OutputDebugStringA("\r\n");
}

void NS_Yutils::_trace(const wchar_t *format, ...)
{
	wchar_t buffer[TRACE_BUFFER_LEN];
	memset(buffer, 0, TRACE_BUFFER_LEN * sizeof(wchar_t));

	time_t t = time(NULL);
	struct tm tmCur;
	localtime_s(&tmCur, &t); 
	wsprintfW(buffer, L"%04d-%02d-%02d %02d:%02d:%02d ", 
		tmCur.tm_year + 1900, 
		tmCur.tm_mon + 1,
		tmCur.tm_mday,
		tmCur.tm_hour,
		tmCur.tm_min,
		tmCur.tm_sec);

	va_list argptr;
	va_start(argptr, format);
	wvsprintfW(buffer + DATETIME_LEN, format, argptr);
	va_end(argptr);

	OutputDebugStringW(buffer);
	OutputDebugStringW(L"\r\n");
}