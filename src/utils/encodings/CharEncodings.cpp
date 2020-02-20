#include "pch.h"
#include "CharEncodings.h"
#include <memory>
#pragma warning(disable:4018)

std::string NS_Yutils::WCharToUtf8(const std::wstring& sSrc)
{
	std::string sRet;
	WCharToUtf8(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::string NS_Yutils::WCharToUtf8(const wchar_t* pSrc, size_t numChars)
{
	std::string sRet;
	WCharToUtf8(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::WCharToUtf8(std::string& sDst, const std::wstring& sSrc)
{
	WCharToUtf8(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::WCharToUtf8(std::string& sDst, const wchar_t* pSrc, size_t numChars)
{
	auto len = WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);//including null terminator
	if (len <= 0)
		throw std::runtime_error("WideCharToMultiByte error");

	std::unique_ptr<char[]> pDst(new char[len]());
	WideCharToMultiByte(CP_UTF8, 0, pSrc, numChars, pDst.get(), len, NULL, NULL);//not including null terminator, but the position is valueinitialized
	sDst = pDst.get();	
}

std::string NS_Yutils::WCharToMB(const std::wstring& sSrc)
{
	std::string sRet;
	WCharToMB(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::string NS_Yutils::WCharToMB(const wchar_t* pSrc, size_t numChars)
{
	std::string sRet;
	WCharToMB(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::WCharToMB(std::string& sDst, const std::wstring& sSrc)
{
	WCharToMB(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::WCharToMB(std::string& sDst, const wchar_t* pSrc, size_t numChars)
{
	auto len = WideCharToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	if (len <= 0)
		throw std::runtime_error("WideCharToMultiByte error");

	std::unique_ptr<char[]> pDst(new char[len]());
	len = WideCharToMultiByte(CP_ACP, 0, pSrc, numChars, pDst.get(), len, NULL, NULL);
	sDst = pDst.get();
}

std::string NS_Yutils::MBToUtf8(const std::string& sSrc)
{
	std::string sRet;
	MBToUtf8(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::string NS_Yutils::MBToUtf8(const char* pSrc, size_t numChars)
{
	std::string sRet;
	MBToUtf8(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::MBToUtf8(std::string& sDst, const std::string& sSrc)
{
	MBToUtf8(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::MBToUtf8(std::string& sDst, const char* pSrc, size_t numChars)
{
	auto wlen = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	if (wlen <= 0)
		throw std::runtime_error("MultiByteToWideChar error");

	std::unique_ptr<wchar_t[]> pWide(new wchar_t[wlen]());
	MultiByteToWideChar(CP_ACP, 0, pSrc, numChars, pWide.get(), wlen);

	auto len = WideCharToMultiByte(CP_UTF8, 0, pWide.get(), -1, NULL, 0, NULL, NULL);
	if (len <= 0)
		throw std::runtime_error("WideCharToMultiByte error");

	std::unique_ptr<char[]> pDst(new char[len]());
	WideCharToMultiByte(CP_UTF8, 0, pWide.get(), -1, pDst.get(), len, NULL, NULL);
	sDst = pDst.get();
}

std::wstring NS_Yutils::MBToWChar(const std::string& sSrc)
{
	std::wstring sRet;
	MBToWChar(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::wstring NS_Yutils::MBToWChar(const char* pSrc, size_t numChars)
{
	std::wstring sRet;
	MBToWChar(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::MBToWChar(std::wstring& sDst, const std::string& sSrc)
{
	MBToWChar(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::MBToWChar(std::wstring& sDst, const char* pSrc, size_t numChars)
{
	auto len = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	if (len <= 0)
		throw std::logic_error("MultiByteToWideChar error");

	std::unique_ptr<wchar_t[]> pDst(new wchar_t[len]());
	MultiByteToWideChar(CP_ACP, 0, pSrc, numChars, pDst.get(), len);
	sDst = pDst.get();
}

std::wstring NS_Yutils::Utf8ToWChar(const std::string& sSrc)
{
	std::wstring sRet;
	Utf8ToWChar(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::wstring NS_Yutils::Utf8ToWChar(const char* pSrc, size_t numChars)
{
	std::wstring sRet;
	Utf8ToWChar(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::Utf8ToWChar(std::wstring& sDst, const std::string& sSrc)
{
	Utf8ToWChar(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::Utf8ToWChar(std::wstring& sDst, const char* pSrc, size_t numChars)
{
	auto len = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	if (len <= 0)
		throw std::logic_error("MultiByteToWideChar error");

	std::unique_ptr<wchar_t[]> pDst(new wchar_t[len]());
	MultiByteToWideChar(CP_UTF8, 0, pSrc, numChars, pDst.get(), len);
	sDst = pDst.get();
}

std::string NS_Yutils::Utf8ToMB(const std::string& sSrc)
{
	std::string sRet;
	Utf8ToMB(sRet, sSrc.c_str(), sSrc.length());
	return sRet;
}

std::string NS_Yutils::Utf8ToMB(const char* pSrc, size_t numChars)
{
	std::string sRet;
	Utf8ToMB(sRet, pSrc, numChars);
	return sRet;
}

void NS_Yutils::Utf8ToMB(std::string& sDst, const std::string& sSrc)
{
	Utf8ToMB(sDst, sSrc.c_str(), sSrc.length());
}

void NS_Yutils::Utf8ToMB(std::string& sDst, const char* pSrc, size_t numChars)
{
	auto wlen = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	if (wlen <= 0) 
		throw std::logic_error("MultiByteToWideChar error");

	std::unique_ptr<wchar_t[]> pWide(new wchar_t[wlen]());
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, pWide.get(), wlen);

	auto len = WideCharToMultiByte(CP_ACP, 0, pWide.get(), -1, NULL, 0, NULL, NULL);
	if(len <= 0)
		throw std::logic_error("WideCharToMultiByte error");

	std::unique_ptr<char[]> pDst(new char[len]());
	WideCharToMultiByte(CP_ACP, 0, pWide.get(), -1, pDst.get(), len, NULL, NULL);
	sDst = pDst.get();
}