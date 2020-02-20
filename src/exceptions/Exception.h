#pragma once
#include "..\Macros\ttype.h"

class YException : public std::exception
{
public:
	explicit YException(const char* message = "", const char* file = "<unknown file>", const char* func = "<unknown func>", int line = -1, const char* type = "YException") noexcept;
	//virtual ~YLogicException() = default;

public:
	virtual std::string toString() const;
	const char* what() const noexcept override;				

protected:
	std::string sType;
	mutable std::string sWhat;
	std::string sMessage;
	std::string sFile;
	std::string sFunc;
	int nLine;
};

