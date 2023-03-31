#ifndef STRING_NUMBER_H
#define STRING_NUMBER_H

#include <string>
#include <deque>
#include <unordered_map>
using namespace std;

class StringNumber
{
public:
    // StringNumber();
    StringNumber(const string& strnumber, int base);

    void setData(const string& strnumber, int base);

    bool operator <  (const StringNumber& right);
    bool operator == (const StringNumber& right);
    bool operator <=  (const StringNumber& right);

    bool operator >  (const StringNumber& right);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // unsigned int
    static const StringNumber s_unsignedIntMax8;
    static const StringNumber s_unsignedIntMax10;
    static const StringNumber s_unsignedIntMax16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // signed int
    static const StringNumber s_signedIntMax8;
    static const StringNumber s_signedIntMax10;
    static const StringNumber s_signedIntMax16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // unsigned long
    static const StringNumber s_unsignedLongMax8;
    static const StringNumber s_unsignedLongMax10;
    static const StringNumber s_unsignedLongMax16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // signed long
    static const StringNumber s_signedLongMax8;
    static const StringNumber s_signedLongMax10;
    static const StringNumber s_signedLongMax16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // unsigned long long
    static const StringNumber s_unsignedLongLongMax8;
    static const StringNumber s_unsignedLongLongMax10;
    static const StringNumber s_unsignedLongLongMax16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // signed long long
    static const StringNumber s_signedLongLongMax8;
    static const StringNumber s_signedLongLongMax10;
    static const StringNumber s_signedLongLongMax16;


	static const unordered_map<char, deque<int> > s_OctMap;
	static const unordered_map<char, deque<int> > s_HexMap;
protected:
    void processBase8();
    void processBase10();
    void processBase16();

	size_t oct2Bin();
	size_t dec2Bin();
	size_t hex2Bin();

    string m_strNumber;
    string m_stripedNumber;
    int    m_base;
	deque<int> m_binaryDigitList;
public:
    string getStrNumber() const;
	size_t convert2Bin();
	void truncateToTargetSize(size_t targetSz);
	unsigned long      getULongValue();
	unsigned long long getULongLongValue();
};

#endif