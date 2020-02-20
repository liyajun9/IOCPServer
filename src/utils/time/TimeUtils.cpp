#include "pch.h"
#include "TimeUtils.h"
#include <time.h>
#include <sys\timeb.h>
#include "..\encodings\charEncodings.h"
#pragma warning(disable:4996)

std::wstring NS_Yutils::getCurrDateW(DATE_FORMAT dformat)
{
	return MBToWChar(getCurrDateA(dformat));
}

std::string NS_Yutils::getCurrDateA(DATE_FORMAT dformat)
{
	time_t t(0);
	time(&t);
	tm *pTm = localtime(&t);

	char pszTmp[32]; memset(pszTmp, 0, 32);
	switch (dformat)
	{
	case Date_Format_0:
		sprintf(pszTmp, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_1:
		sprintf(pszTmp, "%02d-%02d-%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_2:
		sprintf(pszTmp, "%04d%02d%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_3:
		sprintf(pszTmp, "%02d%02d%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_4:
		sprintf(pszTmp, "%04d %02d %02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_5:
		sprintf(pszTmp, "%02d %02d %02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_6:
		sprintf(pszTmp, "%04d/%02d/%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_7:
		sprintf(pszTmp, "%02d/%02d/%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_8:
		sprintf(pszTmp, "%02d/%02d/%04d", pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_year + 1900);
		break;
	case Date_Format_9:
		sprintf(pszTmp, "%02d/%02d/%02d", pTm->tm_mon + 1, pTm->tm_mday, (pTm->tm_year + 1900)%1000);
		break;
	case Date_Format_10:
		sprintf(pszTmp, "%02d/%02d/%04d", pTm->tm_mday, pTm->tm_mon + 1, pTm->tm_year + 1900);
		break;
	case Date_Format_11:
		sprintf(pszTmp, "%02d/%02d/%02d", pTm->tm_mday, pTm->tm_mon + 1, (pTm->tm_year + 1900)%1000);
		break;
	default:
		sprintf(pszTmp, "invalid DateFormat");
	}
	return std::string(pszTmp);
}

std::wstring NS_Yutils::getCurrTimeW(bool bMilliSeconds)
{
	return MBToWChar(getCurrTimeA(bMilliSeconds));
}

std::string NS_Yutils::getCurrTimeA(bool bMilliSeconds)
{
	timeb tb;
	ftime(&tb);

	tm *pTm = localtime(&tb.time);

	char pszTmp[32]; memset(pszTmp, 0, 32);
	if(bMilliSeconds)
		sprintf(pszTmp, "%02d:%02d:%02d.%03d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec, tb.millitm);
	else
		sprintf(pszTmp, "%02d:%02d:%02d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);

	return std::string(pszTmp);
}

std::wstring NS_Yutils::getCurrDateTimeW(DATE_FORMAT dformat, bool bMilliSeconds)
{
	return getCurrDateW(dformat) + L" " + getCurrTimeW(bMilliSeconds);
}

std::string NS_Yutils::getCurrDateTimeA(DATE_FORMAT dformat, bool bMilliSeconds)
{
	return getCurrDateA(dformat) + " " + getCurrTimeA(bMilliSeconds);
}

std::wstring NS_Yutils::getAddedDateW(int nAddedDays, DATE_FORMAT dformat)
{
	return MBToWChar(getAddedDateA(nAddedDays, dformat));
}

std::string NS_Yutils::getAddedDateA(int nAddedDays, DATE_FORMAT dformat)
{
	time_t t(0);
	time(&t);
	t += 24 * 3600 * nAddedDays;
	tm *pTm = localtime(&t);

	char pszTmp[32]; memset(pszTmp, 0, 32);
	switch (dformat)
	{
	case Date_Format_0:
		sprintf(pszTmp, "%04d-%02d-%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_1:
		sprintf(pszTmp, "%02d-%02d-%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_2:
		sprintf(pszTmp, "%04d%02d%02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_3:
		sprintf(pszTmp, "%02d%02d%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_4:
		sprintf(pszTmp, "%04d %02d %02d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_5:
		sprintf(pszTmp, "%02d %02d %02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_7:
		sprintf(pszTmp, "%02d/%02d/%02d", (pTm->tm_year + 1900)%1000, pTm->tm_mon + 1, pTm->tm_mday);
		break;
	case Date_Format_8:
		sprintf(pszTmp, "%02d/%02d/%04d", pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_year + 1900);
		break;
	case Date_Format_9:
		sprintf(pszTmp, "%02d/%02d/%02d", pTm->tm_mon + 1, pTm->tm_mday, (pTm->tm_year + 1900)%1000);
		break;
	case Date_Format_10:
		sprintf(pszTmp, "%02d/%02d/%04d", pTm->tm_mday, pTm->tm_mon + 1, pTm->tm_year + 1900);
		break;
	case Date_Format_11:
		sprintf(pszTmp, "%02d/%02d/%02d", pTm->tm_mday, pTm->tm_mon + 1, (pTm->tm_year + 1900)%1000);
		break;

	}
	return std::string(pszTmp);
}