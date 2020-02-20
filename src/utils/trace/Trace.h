// trace
// Example: 
//		YTRACE(_T("debug output variable value = %d"), var);
//
//      will output:
//			2018-07-01 11:33:20 debug output variable value = 5
#pragma once
#include <time.h>

#ifdef _DEBUG
#define YTRACE NS_Yutils::_trace
#else
#define YTRACE(x)
#endif

constexpr int TRACE_BUFFER_LEN = 1024;
constexpr int DATETIME_LEN = 20;

namespace NS_Yutils{

void _trace(const char *format, ...);
void _trace(const wchar_t *format, ...);

}
