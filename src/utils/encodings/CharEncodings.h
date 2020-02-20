/*
Tools for character encoding convertion, support:
	MultiByte <-> Unicode
	Unicode <-> UTF8
	UTF8 <-> MultiByte
*/
//liyajun
#pragma once
#include <string>
#include "..\..\Macros/ttype.h"

namespace NS_Yutils {

	std::string WCharToUtf8(const std::wstring& sSrc);
	std::string WCharToUtf8(const wchar_t* pSrc, size_t numChars);
	void WCharToUtf8(std::string& sDst, const std::wstring& sSrc);
	void WCharToUtf8(std::string& sDst, const wchar_t* pSrc, size_t numChars);

	std::string WCharToMB(const std::wstring& sSrc);
	std::string WCharToMB(const wchar_t* pSrc, size_t numChars);
	void WCharToMB(std::string& sDst, const std::wstring& sSrc);
	void WCharToMB(std::string& sDst, const wchar_t* pSrc, size_t numChars);

	std::string MBToUtf8(const std::string& sSrc);
	std::string MBToUtf8(const char* pSrc, size_t numChars);
	void MBToUtf8(std::string& sDst, const std::string& sSrc);
	void MBToUtf8(std::string& sDst, const char* pSrc, size_t numChars);

	std::wstring MBToWChar(const std::string& sSrc);
	std::wstring MBToWChar(const char* pSrc, size_t numChars);
	void MBToWChar(std::wstring& sDst, const std::string& sSrc);
	void MBToWChar(std::wstring& sDst, const char* pSrc, size_t numChars);

	std::wstring Utf8ToWChar(const std::string& sSrc);
	std::wstring Utf8ToWChar(const char* pSrc, size_t numChars);
	void Utf8ToWChar(std::wstring& sDst, const std::string& sSrc);
	void Utf8ToWChar(std::wstring& sDst, const char* pSrc, size_t numChars);

	std::string Utf8ToMB(const std::string& sSrc);
	std::string Utf8ToMB(const char* pSrc, size_t numChars);
	void Utf8ToMB(std::string& sDst, const std::string& sSrc);
	void Utf8ToMB(std::string& sDst, const char* pSrc, size_t numChars);

	//convert character to hex style
	inline char CharToHex(unsigned char x){ return x > 9 ? x + 55 : x + 48; }
};