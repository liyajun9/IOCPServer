#pragma once
#include <vector>
#include <memory>

class YSessionData {
public:
	YSessionData(size_t nSize);
	std::shared_ptr<void> getSessionData(size_t nIndex);
	void clear();

private:
	const size_t size;
	std::vector<std::shared_ptr<void>> dataVec;
};