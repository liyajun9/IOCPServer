#include "pch.h"
#include "SessionData.h"

YSessionData::YSessionData(size_t nSize)
	:size(nSize), dataVec(size)
{

}

std::shared_ptr<void> YSessionData::getSessionData(size_t nIndex)
{
	return dataVec[nIndex];
}

void YSessionData::clear()
{
	dataVec.clear();
	dataVec.assign(size, std::shared_ptr<void>());
}
