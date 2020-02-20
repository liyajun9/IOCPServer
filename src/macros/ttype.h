#pragma once
#include <string>
#include <tchar.h>
#include <sstream>

#if defined(UNICODE) || defined(_UNICODE)

#define tstring std::wstring
#define tcout std::wcout
#define tcin std::wcin
#define tostream std::wostream
#define tofstream std::wofstream
#define tifstream std::wifstream
#define tfstream std::wfstream
#define tistringstream std::wistringstream
#define tostringstream std::wostringstream

#else

#define tstring std::string
#define tcout std::cout
#define tcin std::cin
#define tostream std::ostream
#define tofstream std::ofstream
#define tifstream std::ifstream
#define tfstream std::fstream
#define tistringstream std::istringstream
#define tostringstream std::ostringstream

#endif

#define ASSERT_STRINGTYPE(T)  static_assert(std::is_same<T, std::string>::value||std::is_same<T, std::wstring>::value, "type must be string or wstring")