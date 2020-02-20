#pragma once
#include <list>
#include <memory>
#include <algorithm>
#include <exceptions\Exception.h>

template <typename T> class YCachedObj {
public:
	virtual ~YCachedObj() = default;

	void* operator new(size_t sz) {
		if (sz != sizeof(T))
			throw YException("incorret type", "cachedObj", "new");

		if (freeList.size() == 0) {
			T* objArr = allocator.allocate(chunk);
			for (size_t i = 0; i < chunk; ++i) {
				freeList.push_back(&objArr[i]);
			}
		}
		T* pRet = *(freeList.begin());
		inuseList.splice(inuseList.end(), freeList, freeList.begin());
		return pRet;
	}
	void operator delete(void* p/*, size_t*/) {
		if (p) {
			auto it = std::find(inuseList.begin(), inuseList.end(), static_cast<T*>(p));
			if (it != inuseList.end())
				freeList.splice(freeList.end(), inuseList, it);
		}
	}

protected:
	static std::allocator<T> allocator;
	static const size_t chunk;

	static std::list<T*> freeList;
	static std::list<T*> inuseList;
};