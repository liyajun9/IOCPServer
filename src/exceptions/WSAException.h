#pragma once
#include <concrt.h>
#include "..\Macros\ttype.h"
#include "Exception.h"

class YWSAException : public YException
{
public:
	explicit YWSAException(const char* file = "<unknown file>", const char* func = "<unknown func>", int line = -1, const char* type = "YWSAException") noexcept;

public:
	std::string toString() const override;
	const char* what() const noexcept override;

protected:
	std::string TranslateErrCode(DWORD errCode);

private:
	DWORD dwErrCode;
};
