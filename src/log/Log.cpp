#include "pch.h"
#include "Log.h"

#pragma warning(disable:4996)

YLog yLog(_T("log"), true,  ELogItem::ALL, 3);

YLog::YLog(tstring sDir, bool bAutoEndline /*= false*/, ELogItem nLogItem /*= ELogItem::ALL*/, int nDays /*= 7*/)
	:sDirectory(
#if defined(UNICODE) || defined(_UNICODE)
		sDir
#else
		NS_Yutils::MBToWChar(sDir)
#endif
	)
	,m_bAutoEndline(bAutoEndline)
	,logItem(nLogItem)
	,nNumExpireDays(nDays)
{
	InitializeCriticalSection(&m_cs);
	sToday = getCurrDateW(NS_Yutils::Date_Format_3);
}

YLog::~YLog()
{
	m_fstream.close();
	DeleteCriticalSection(&m_cs);
}

#define WRITE_ARGLISTA(TYPE, FORMAT) \
{\
if(!(LOG_LEVEL & TYPE)) return;\
va_list argList;\
va_start(argList, FORMAT);\
auto len = _vscprintf(FORMAT, argList);\
std::unique_ptr<char[]> spData(new char[len+1]());\
vsprintf(spData.get(), FORMAT, argList);\
va_end(argList);\
std::wstring sData;\
NS_Yutils::MBToWChar(sData, spData.get(), len);\
write(TYPE, sData.c_str());\
}

#define WRITE_ARGLISTW(TYPE, FORMAT) \
{\
if(!(LOG_LEVEL & TYPE)) return;\
va_list argList;\
va_start(argList, FORMAT);\
auto len = _vscwprintf(FORMAT, argList);\
std::unique_ptr<wchar_t[]> spData(new wchar_t[len + 1]()); \
vswprintf(spData.get(), FORMAT, argList);\
va_end(argList);\
write(TYPE, spData.get());\
}

void YLog::LogInfo(const char* format, ...){	WRITE_ARGLISTA(ELogType::ELog_Info, format);	}
void YLog::LogInfo(const wchar_t* format, ...){	WRITE_ARGLISTW(ELogType::ELog_Info, format);	}
void YLog::LogDebug(const char* format, ...){	WRITE_ARGLISTA(ELogType::ELog_Debug, format);	}
void YLog::LogDebug(const wchar_t* format, ...){	WRITE_ARGLISTW(ELogType::ELog_Debug, format);	}
void YLog::LogWarn(const char* format, ...){	WRITE_ARGLISTA(ELogType::ELog_Warn, format);	}
void YLog::LogWarn(const wchar_t* format, ...){	WRITE_ARGLISTW(ELogType::ELog_Warn, format);	}
void YLog::LogError(const char* format, ...){	WRITE_ARGLISTA(ELogType::ELog_Warn, format);	}
void YLog::LogError(const wchar_t* format, ...){	WRITE_ARGLISTW(ELogType::ELog_Error, format);	}
void YLog::LogFatal(const char* format, ...){	WRITE_ARGLISTA(ELogType::ELog_Fatal, format);	}
void YLog::LogFatal(const wchar_t* format, ...){	WRITE_ARGLISTW(ELogType::ELog_Fatal, format);	}
void YLog::Log(ELogType logType, const char* format, ...){	WRITE_ARGLISTA(logType, format);	}
void YLog::Log(ELogType logType, const wchar_t* format, ...){	WRITE_ARGLISTW(logType, format);	}

void YLog::write(ELogType logLevel, const wchar_t* pData)
{
	YCSLocker locker(m_cs);

	static bool bFirstRun = true;
	bool bNewDay = false;

	std::wstring sCurrDate = getCurrDateW(NS_Yutils::Date_Format_3);
	 bNewDay = sToday.compare(sCurrDate)!=0;

	 //delete expired
	if(bFirstRun || bNewDay){
		delExpired();
	}

	//open file stream
	if (!m_fstream.is_open() || bNewDay) {
		if (bNewDay) {
			m_fstream.close();
			m_fstream.clear();
			bFirstRun = true;
		}
		sToday = sCurrDate;
		std::wstring sPath;
		if (sDirectory.empty()) {
			sPath = sToday + L".log";
		}else {
			_wmkdir(sDirectory.c_str());
			sPath = sDirectory + L"\\" + sToday + L".log";
		}
		m_fstream.open(sPath.c_str(), std::ofstream::out | std::ofstream::app | std::ios::binary);
		m_fstream.imbue(std::locale("chs"));
	}
	if (!m_fstream.is_open()) return;

	m_strstr.str(_T(""));
	if(bFirstRun){
		m_strstr << L"\r\n********************************New Log*********************************" << std::endl;
		bFirstRun = false;
	}	
	switch (logLevel)
	{
	case ELogType::ELog_Debug:
		m_strstr << L"debug ";
		break;
	case ELogType::ELog_Info:
		m_strstr << L"info ";
		break;
	case ELogType::ELog_Warn:
		m_strstr << L"warn ";
		break;
	case ELogType::ELog_Error:
		m_strstr << L"error ";
		break;
	case ELogType::ELog_Fatal:
		m_strstr << L"fatal ";
		break;
	default:
		m_strstr << L"unknown ";
		break;
	}

	if(static_cast<int>(logItem) & static_cast<int>(ELogItem::DATETIME)){
		m_strstr << getCurrDateTimeW(NS_Yutils::Date_Format_0, true);
	}
	if(static_cast<int>(logItem) & static_cast<int>(ELogItem::THREADID)){
		m_strstr << L"["<<GetCurrentThreadId() << "]";
	}
	
	if(m_bAutoEndline)
		m_strstr << L" " << pData << std::endl;
	else
		m_strstr << L" " << pData;

	m_fstream << m_strstr.str() << std::flush;
}

void YLog::delExpired()
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW ffd;

	std::wstring sWildcard = sDirectory;
	sWildcard.append(L"\\*.*");
	hFind = FindFirstFileW(sWildcard.c_str(), &ffd);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	wchar_t pszFile[MAX_PATH];
	std::wstring::size_type len = sDirectory.length();
	memset(pszFile, 0, MAX_PATH * sizeof(wchar_t));
	memcpy(pszFile, sDirectory.c_str(), len * sizeof(wchar_t));
	do{
		if(wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0){
			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				continue;
			}else{
				if(isExpired(ffd.cFileName)){
					memset(pszFile + len, 0, (MAX_PATH - len) * sizeof(wchar_t));
					memcpy(pszFile + len, L"\\", sizeof(wchar_t));
					memcpy(pszFile + len + 1, ffd.cFileName, wcslen(ffd.cFileName) * sizeof(wchar_t));
					_wremove(pszFile);
				}
			}
		}
	}while(FindNextFileW(hFind, &ffd) != 0);

	FindClose(hFind);
	hFind = INVALID_HANDLE_VALUE;
}

bool YLog::isExpired(std::wstring sFileName)
{
	auto nPos = sFileName.find_last_of('.');
	if(nPos != std::wstring::npos){
		sFileName = sFileName.substr(0,nPos);
	}

	if(sFileName.length() == 6){//length of log filename
		if(sFileName.find_first_not_of(L"0123456789") == std::wstring::npos){
			std::wstring sExpireDate = NS_Yutils::getAddedDateW(0 - nNumExpireDays, NS_Yutils::Date_Format_3);
			if(sFileName.compare(sExpireDate) > 0)
				return false;
		}
	}
	 return true;
}