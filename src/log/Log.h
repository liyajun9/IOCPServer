//  Logger
//Note: character encoding of log file is GB2312
//liyajun

#pragma once

#include "..\Macros\ttype.h"
#include <Windows.h>
#include <fstream>
#include <iosfwd>
#include <stdarg.h>
#include <direct.h>
#include <xlocale>
#include <wchar.h>
#include <sstream>
#include "..\utils\time\TimeUtils.h"
#include "..\utils\synchronize\CSLocker.h"
#include "..\utils\encodings\CharEncodings.h"

#pragma warning(disable:4996)

//use these macros to make logging
#define LOGDEBUG yLog.LogDebug
#define LOGINFO yLog.LogInfo
#define LOGWARN yLog.LogWarn
#define LOGERROR yLog.LogError
#define LOGFATAL yLog.LogFatal

//Log levels and types and default log level
enum ELogType //use old enum type in order to have bit operation
{
	ELog_Fatal = 0x01,
	ELog_Error = 0x02,
	ELog_Warn = 0x04,
	ELog_Info = 0x08,
	ELog_Debug = 0x10
};
constexpr int LOG_LEVEL_FATAL = ELogType::ELog_Fatal;
constexpr int LOG_LEVEL_ERROR = (LOG_LEVEL_FATAL | ELogType::ELog_Error);
constexpr int LOG_LEVEL_WARN = (LOG_LEVEL_ERROR | ELogType::ELog_Warn);
constexpr int LOG_LEVEL_INFO = (LOG_LEVEL_WARN | ELogType::ELog_Info);
constexpr int LOG_LEVEL_DEBUG = (LOG_LEVEL_INFO | ELogType::ELog_Debug);

#if(defined DEBUG || defined _DEBUG)
constexpr int LOG_LEVEL = LOG_LEVEL_DEBUG;
#else
constexpr int LOG_LEVEL = LOG_LEVEL_INFO;
#endif

//log items
enum class ELogItem
{
	DATETIME = 0x01,
	THREADID = 0x02,
	ALL = 0x03
};

class YLog{
public:
	explicit YLog(tstring sDir, bool bAutoEndline = false, ELogItem nLogItem = ELogItem::ALL, int nDays = 7);
	~YLog();

	void LogDebug(const char* format, ...);
	void LogDebug(const wchar_t* format, ...);
	void LogInfo(const char* format, ...);
	void LogInfo(const wchar_t* format, ...);
	void LogWarn(const char* format, ...);
	void LogWarn(const wchar_t* format, ...);
	void LogError(const char* format, ...);
	void LogError(const wchar_t* format, ...);
	void LogFatal(const char* format, ...);
	void LogFatal(const wchar_t* format, ...);

	void Log(ELogType logType, const char* format, ...);	
	void Log(ELogType logType, const wchar_t* format, ...);

private:
	void write(ELogType logLevel, const wchar_t* pData);
	void delExpired();
	bool isExpired(std::wstring sFileName);

private:	
	bool m_bAutoEndline;
	ELogItem logItem;
	int nNumExpireDays;
	std::wstring sDirectory;
	std::wstring sToday;

	std::wofstream m_fstream;
	std::wostringstream m_strstr;

	CRITICAL_SECTION m_cs;
};

extern YLog yLog;