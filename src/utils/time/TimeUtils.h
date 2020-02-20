#pragma once
#include <time.h>
#include <string>
#include "..\..\Macros\ttype.h"

#if defined(UNICODE) || defined(_UNICODE)
#define getCurrDate getCurrDateW
#define getCurrTime getCurrTimeW
#define getCurrDateTime getCurrDateTimeW
#define getAddedDate getAddedDateW
#else
#define getCurrDate getCurrDateA
#define getCurrTime getCurrTimeA
#define getCurrDateTime getCurrDateTimeA
#define getAddedDate getAddedDateA
#endif
namespace NS_Yutils{

	enum DATE_FORMAT{
		Date_Format_0,	//yyyy-mm-dd
		Date_Format_1,	//yy-mm-dd
		Date_Format_2,	//yyyymmdd
		Date_Format_3,	//yymmdd
		Date_Format_4,	//yyyy mm dd
		Date_Format_5,	//yy mm dd
		Date_Format_6,	//yyyy/mm/dd
		Date_Format_7,	//yy/mm/dd
		Date_Format_8,	//mm/dd/yyyy
		Date_Format_9,	//mm/dd/yy
		Date_Format_10,	//dd/mm/yyyy
		Date_Format_11	//dd/mm/yy
	};

	std::string getCurrDateA(DATE_FORMAT dformat = Date_Format_0);
	std::wstring getCurrDateW(DATE_FORMAT dformat = Date_Format_0);
	std::string getCurrTimeA(bool bMilliSeconds = false);
	std::wstring getCurrTimeW(bool bMilliSeconds = false);
	std::string getCurrDateTimeA(DATE_FORMAT dformat = Date_Format_0, bool bMilliSeconds = false);
	std::wstring getCurrDateTimeW(DATE_FORMAT dformat = Date_Format_0, bool bMilliSeconds = false);
	std::string getAddedDateA(int nAddedDays, DATE_FORMAT dformat = Date_Format_0);
	std::wstring getAddedDateW(int nAddedDays, DATE_FORMAT dformat = Date_Format_0);
};