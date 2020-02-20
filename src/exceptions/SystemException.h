#pragma once
#include "..\Macros\ttype.h"
#include "Exception.h"
#include <concrt.h>

class YSystemException : public YException
{
public:
	explicit YSystemException(const char* file = "<unknown file>", const char* func = "<unknown func>", int line = -1, const char* type = "YSystemException") noexcept;

public:
	std::string toString() const override;
	const char* what() const noexcept override;

protected:
	std::string TranslateErrCode(DWORD errCode);

private:
	DWORD dwErrCode;
};
