#pragma once
#include "sqlite3.h"
#include <vector>
#include "..\..\Macros\ttype.h"

#if defined(UNICODE) || defined(_UNICODE)
#define TOUTF8		NS_Yutils::WCharToUtf8
#define FROMUTF8	NS_Yutils::Utf8ToWChar
#define TOMB		NS_Yutils::WCharToMB
#else
#define TOUTF8		NS_Yutils::MBToUtf8
#define FROMUTF8	NS_Yutils::Utf8ToMB
#define TOMB
#endif

constexpr int SQLITE_QUERY_SUCCESS = 0;
constexpr int SQLITE_QUERY_ERROR = 1;

using TransactSQLs = std::vector<tstring>;

#define SQLite YSQLite::GetInstance()

class YSQLite{
public:
	static YSQLite& GetInstance();

	bool GetIsExist(const tstring& sSQL);
	int GetIntField(const tstring& sSQL);//return -1 on error or empty result
	tstring GetStringField(const tstring& sSQL); //return "" on error or empty result

	bool ExecuteSQL(const tstring& sSQL, unsigned int *pAffectedRows = NULL);
	bool ExecuteTransac(const TransactSQLs& vecSQL);

	bool GetIsTableExist(const tstring& sTable);
	bool GetIsColumnExist(const tstring& sTable, const tstring&sCol);//note: column name is sensitive

	int GetLastError();//0(SQLITE_QUERY_SUCCESS) or 1(SQLITE_QUERY_ERROR)

private:
	void InitConnection();

private:	
	YSQLite();
	~YSQLite();

private:
	static sqlite3 *pDbconnection;
	static const char *pDbfilename;
	CRITICAL_SECTION m_cs;  

	int nLastError; //0 - success    other-error
};