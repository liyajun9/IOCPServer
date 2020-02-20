#include "pch.h"
#include "SQliteException.h"

YSQLiteException::YSQLiteException(const char* message/*=""*/, const char* file/*="<unknown file>"*/,
	const char* func /*="<unknown func>"*/, int line/* = -1*/, const char* type /*= "YException"*/) noexcept
	:YException(message, file, func, line, type)
{
}