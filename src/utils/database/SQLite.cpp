#include "pch.h"
#include "SQLite.h"
#include "..\..\log\Log.h"
#include "..\..\exceptions\SQliteException.h"
#include <sstream>
#include "..\encodings\CharEncodings.h"
#include "..\synchronize\cslocker.h"

#pragma comment(lib, "sqlite3.lib")

sqlite3* YSQLite::pDbconnection = NULL;
const char* YSQLite::pDbfilename = "netbar.db";

YSQLite& YSQLite::GetInstance()
{
	static YSQLite dbSqliteInstance;
	return dbSqliteInstance;
}

YSQLite::YSQLite() 
	:nLastError(0)
{
	::InitializeCriticalSection(&m_cs);
}

YSQLite::~YSQLite()
{
	sqlite3_close(pDbconnection);
	::DeleteCriticalSection(&m_cs);
}

bool YSQLite::ExecuteSQL(const tstring& sSQL, unsigned int *pAffectedRows /*= NULL*/)
{
	std::string sSQLutf8 = TOUTF8(sSQL);

	YCSLocker locker(m_cs); 
	char *pszErrMsg = NULL;
	try{
		InitConnection();
		if(!pDbconnection)
			throw YSQLiteException("invalid connection!", TOMB(sSQL).c_str(), "ExecuteSQL");

		int nRes(0);
		if(nRes = sqlite3_exec(pDbconnection, sSQLutf8.c_str(), NULL, NULL, &pszErrMsg) != SQLITE_OK)
			throw YSQLiteException(pszErrMsg, TOMB(sSQL).c_str(), "ExecuteSQL");

		if(pAffectedRows)
			*pAffectedRows = sqlite3_changes(pDbconnection);

		nLastError = SQLITE_QUERY_SUCCESS;
	}catch(YSQLiteException& e){
		LOGERROR(e.what());
		sqlite3_free(pszErrMsg);
		nLastError = SQLITE_QUERY_ERROR;
		return false;
	}

	if(pszErrMsg)		sqlite3_free(pszErrMsg);	
	return true;
}

bool YSQLite::GetIsExist(const tstring& sSQL)
{
	std::string sSQLutf8 = TOUTF8(sSQL);

	YCSLocker locker(m_cs);
	sqlite3_stmt *stmt = NULL;
	char *pszErrMsg = NULL;
	bool bRet(false);

	try{
		InitConnection();
		if(!pDbconnection)
			throw YSQLiteException("invalid connection!", TOMB(sSQL).c_str(), "GetIntField");

		int nRes(0);
		if(nRes = sqlite3_prepare_v2(pDbconnection, sSQLutf8.c_str(), -1, &stmt, NULL) != SQLITE_OK)
			throw YSQLiteException("prepare statement error", TOMB(sSQL).c_str(), "GetIntField");

		if(nRes = sqlite3_step(stmt) == SQLITE_ROW){
			bRet = true;
		}else if(nRes == SQLITE_DONE || nRes == SQLITE_OK){
			bRet = false;
		}else{
			std::stringstream ssErr;
			ssErr<<"step statement error("<<nRes<<")";
			throw YSQLiteException(ssErr.str().c_str(), TOMB(sSQL).c_str(), "GetIntField");
		}
		nLastError = SQLITE_QUERY_SUCCESS;
	}catch(YSQLiteException& e){
		bRet = false;
		LOGERROR(e.what());
		nLastError = SQLITE_QUERY_ERROR;
	}

	if(stmt)		sqlite3_finalize(stmt);
	if(pszErrMsg)		sqlite3_free(pszErrMsg);
	return bRet;
}

int YSQLite::GetIntField(const tstring& sSQL)
{
	std::string sSQLutf8 = TOUTF8(sSQL);

	YCSLocker locker(m_cs);
	sqlite3_stmt *stmt = NULL;
	char *pszErrMsg = NULL;
	int nRet(-1);

	try{
		InitConnection();
		if(!pDbconnection)
			throw YSQLiteException("invalid connection!", TOMB(sSQL).c_str(), "GetIntField");

		int nRes(0);
		if(nRes = sqlite3_prepare_v2(pDbconnection, sSQLutf8.c_str(), -1, &stmt, NULL) != SQLITE_OK)
			throw YSQLiteException("prepare statement error", TOMB(sSQL).c_str(), "GetIntField");

		if(nRes = sqlite3_step(stmt) == SQLITE_ROW){
			nRet = sqlite3_column_int(stmt, 0);
		}else if(nRes == SQLITE_DONE || nRes == SQLITE_OK){
			nRet = -1;
		}else{
			std::stringstream ssErr;
			ssErr<<"step statement error("<<nRes<<")";
			throw YSQLiteException(ssErr.str().c_str(), TOMB(sSQL).c_str(), "GetIntField");
		}
		nLastError = SQLITE_QUERY_SUCCESS;
	}catch(YSQLiteException& e){
		nRet = -1;
		LOGERROR(e.what());
		nLastError = SQLITE_QUERY_ERROR;
	}

	if(stmt)		sqlite3_finalize(stmt);
	if(pszErrMsg)		sqlite3_free(pszErrMsg);
	return nRet;
}

tstring YSQLite::GetStringField(const tstring& sSQL)
{
	std::string sSQLutf8 = TOUTF8(sSQL);

	YCSLocker locker(m_cs);
	sqlite3_stmt *stmt = NULL;
	char *pszErrMsg = NULL;
	std::string sRet = "";

	try{
		InitConnection();
		if(!pDbconnection)
			throw YSQLiteException("invalid connection!", TOMB(sSQL).c_str(), "GetIntField");

		int nRes(0);
		if(nRes = sqlite3_prepare_v2(pDbconnection, sSQLutf8.c_str(), -1, &stmt, NULL) != SQLITE_OK)
			throw YSQLiteException("prepare statement error", TOMB(sSQL).c_str(), "GetIntField");

		if(nRes = sqlite3_step(stmt) == SQLITE_ROW){
			sRet = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		}else if(nRes == SQLITE_DONE || nRes == SQLITE_OK){
			sRet = "";
		}else{
			std::stringstream ssErr;
			ssErr<<"step statement error("<<nRes<<")";
			throw YSQLiteException(ssErr.str().c_str(), TOMB(sSQL).c_str(), "GetIntField");
		}
		nLastError = SQLITE_QUERY_SUCCESS;
	}catch(YSQLiteException& e){
		sRet = "";
		LOGERROR(e.what());
		nLastError = SQLITE_QUERY_ERROR;
	}

	if(stmt)		sqlite3_finalize(stmt);
	if(pszErrMsg)		sqlite3_free(pszErrMsg);
	return FROMUTF8(sRet);
}

bool YSQLite::ExecuteTransac(const TransactSQLs& vecSQL)
{
	YCSLocker locker(m_cs);
	sqlite3_stmt *stmt = NULL;
	char *pszErrMsg = NULL;
	bool bRet(false);

	try{
		InitConnection();
		if(!pDbconnection)
			throw YSQLiteException("invalid connection!", "DBSQLite", "ExecuteTransac");

		int nRes(0);
		sqlite3_exec(pDbconnection, "begin", NULL, NULL, &pszErrMsg);

		for(TransactSQLs::const_iterator iter = vecSQL.begin(); iter != vecSQL.end(); ++iter){
			const tstring& sSQL = (tstring)(*iter);
			std::string sSQLutf8 = TOUTF8(sSQL);
			if(nRes = sqlite3_exec(pDbconnection, sSQLutf8.c_str(), NULL, NULL, &pszErrMsg) != SQLITE_OK)
				throw YSQLiteException(pszErrMsg, TOMB(sSQL).c_str(), "ExecuteTransac");
		}

		sqlite3_exec(pDbconnection, "commit", NULL, NULL, NULL);
		nLastError = SQLITE_QUERY_SUCCESS;
	}catch(YSQLiteException& e){
		sqlite3_exec(pDbconnection, "rollback", NULL, NULL, NULL);
		LOGERROR(e.what());
		sqlite3_free(pszErrMsg);
		nLastError = SQLITE_QUERY_ERROR;
		return false;
	}

	if(pszErrMsg)		sqlite3_free(pszErrMsg);	
	return true;
}

bool YSQLite::GetIsTableExist(const tstring& sTable)
{
	tostringstream ssSQL;
	ssSQL<<_T("select count(*) from sqlite_master where type = \'table\' and name = \'")<<sTable<<_T("\'");

	return GetIntField(ssSQL.str()) > 0;
}

bool YSQLite::GetIsColumnExist(const tstring& sTable, const tstring& sCol)
{
	tostringstream ssSQL;
	ssSQL<<_T("select count(*) from PRAGMA_table_info(\'")<<sTable<<_T("\') where name=\'")<<sCol<<_T("\'");

	return GetIntField(ssSQL.str()) > 0;
}

void YSQLite::InitConnection()
{
	if(!pDbconnection)
		sqlite3_open_v2(pDbfilename, &pDbconnection, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
}

int YSQLite::GetLastError()
{
	return nLastError;
}


