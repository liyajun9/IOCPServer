#pragma once
#include "Exception.h"

class YSQLiteException : public YException{
public:
	explicit YSQLiteException(const char* message = "", const char* file = "<unknown file>", 
		const char* func = "<unknown func>", int line = -1, const char* type = "YSQLiteException") noexcept;
};