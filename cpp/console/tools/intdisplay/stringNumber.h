#ifndef STRING_DECIMAL_NUMBER_H
#define STRING_DECIMAL_NUMBER_H

#include <string>

using namespace std;

struct AdditionResultInfo
{
    AdditionResultInfo(int result, int cbit) : res(result), carrybit(cbit) { }
    virtual ~AdditionResultInfo() = default;
    int res;
    int carrybit;
};


class StringNumber
{
public:
    StringNumber(const string& snumber, int base);
    virtual ~StringNumber() = default;
    StringNumber(const  StringNumber& rnum);
    StringNumber& operator = (const StringNumber& right);

    StringNumber operator + (const StringNumber& right);
    StringNumber operator * (const StringNumber& right);

    string getDetail();
protected:
    string base2_16(const string& input);
    string base2_8(const string& input);
    string base2_10(const string& input);

    string base16_2(const string& input);
    string base8_2(const string& input);
    string base10_2(const string& input);

    string binMinus1(const string& input);
    string flipBin(const string& input);

    void resetNumber(const string& snumber, int base);


    // number /= 2;
    pair<StringNumber,int> devide2();

    void decimalMinus1();


    void check2();   // check and remove  suffix "b/B"
    void check8();   // check and remove  prefix "0"
    void check10();
    void check16();   // check and remove prefix "0x or 0X"

    bool isZero();


    int getBinaryPlaceholdZeroCnt(int nBinaryBits);

    string cutbB( const string& input);
    string cutZero( const string& input);
    string cut0x( const string& input);


    string getOriginalStr() const;

    string calcBinary();
    string calcOctal();

    string calcDecimal();
    string calcDecimalWithSignedFlag();

    string calcHex();


    ////////////////////////////////////////////////////
    //
    // Data Member 
    //
    ////////////////////////////////////////////////////

    string m_snumber;
    string m_orignalStr;
    int    m_base;

    static const int s_2to8Bit;
    static const int s_2to16Bit;
    static const int s_nBitsPerByte;

};


#endif
