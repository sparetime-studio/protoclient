
// stdafx.cpp : source file that includes just the standard includes
// client.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <string>



std::string WC2MB(CStringW str) {
	std::string s;
	s.resize(str.GetLength());
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)str, str.GetLength(), const_cast<char*>(s.data()), s.size(), 0, 0);
	return s;
}

CStringW MB2WC(const std::string& s) {
	WCHAR sz[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.length() + 1, sz, MAX_PATH);
	return sz;
}

int reverse(int a)
{
	union {
		int i;
		char c[4];
	} u, r;

	u.i = a;
	r.c[0] = u.c[3];
	r.c[1] = u.c[2];
	r.c[2] = u.c[1];
	r.c[3] = u.c[0];

	return r.i;

}

