#pragma once
#ifndef HEADERS_H
#define HEADERS_H

#include <Winsock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <WinBase.h>

extern "C" {
#include <openssl\applink.c>
#include <openssl\ossl_typ.h>
#include <openssl\evp.h>
};

#pragma comment(lib, "Ws2_32.lib")

#endif