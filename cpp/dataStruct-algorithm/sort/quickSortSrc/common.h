#pragma once

#include <string>
#include <exception>
using namespace std;

class myIdxOutOfRangeException : exception
{
public:
	myIdxOutOfRangeException(const string& msg) : exception(), m_errorMsg(msg) { }
	const char* what() const noexcept override { return m_errorMsg.c_str(); }
protected:
	string m_errorMsg;
};


extern bool isArray_AscendOrdered(int* ary, int sz);
extern bool isArray_DescendOrdered(int* ary, int sz);

extern int getElementSafely(int* ary,int idx, int arySz);

extern void swap2Elements(int* ary, int i, int j);
extern void swap2Elements_Safe(int* ary, int idx1, int idx2, int arySz);