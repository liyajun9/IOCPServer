#include "pch.h"
#include "Exception.h"
#include <sstream>

YException::YException(const char* message/*=""*/, const char* file/*="<unknown file>"*/, 
	const char* func /*="<unknown func>"*/, int line/* = -1*/, const char* type /*= "YException"*/) noexcept
	: std::exception()
	,sMessage(message)
	,sFile(file)
	,sFunc(func)
	,nLine(line)
	,sType(type)
{
}

const char* YException::what() const noexcept
{
	if (sWhat.empty())
		toString();
	return sWhat.c_str();
}

std::string YException::toString() const
{
	if(sWhat.empty()){
		std::stringstream sstr;
		sstr << sType << " caught: "<< sMessage <<".\n\tat " << sFile << "." << sFunc << " line " << nLine;
		sWhat = sstr.str();
	}
	return sWhat;
}