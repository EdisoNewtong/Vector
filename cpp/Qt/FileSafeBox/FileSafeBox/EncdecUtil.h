#ifndef   ENCRYPT_DECRYPT_UTIL_H
#define   ENCRYPT_DECRYPT_UTIL_H

class EncryptDecryptUtil
{
public:
    static char enc1ch(int type, char ch);
    static char dec1ch(int type, char ch);

    static bool enc2ch(int type, char ch1, char ch2, char* pOut1, char* pOut2);
    static bool dec2ch(int type, char ch1, char ch2, char* pOut1, char* pOut2);

protected:
    static char enc1(char ch);
    static char enc2(char ch);
    static char enc3(char ch);
    static char enc4(char ch);
    static char dec1(char ch);
    static char dec2(char ch);
    static char dec3(char ch);
    static char dec4(char ch);

    static void encWord1(char ch1, char ch2, char* pOut1, char* pOut2);
    static void encWord2(char ch1, char ch2, char* pOut1, char* pOut2);
    static void encWord3(char ch1, char ch2, char* pOut1, char* pOut2);
    static void encWord4(char ch1, char ch2, char* pOut1, char* pOut2);
    static void decWord1(char ch1, char ch2, char* pOut1, char* pOut2);
    static void decWord2(char ch1, char ch2, char* pOut1, char* pOut2);
    static void decWord3(char ch1, char ch2, char* pOut1, char* pOut2);
    static void decWord4(char ch1, char ch2, char* pOut1, char* pOut2);
private:
    EncryptDecryptUtil() = delete;
    virtual ~EncryptDecryptUtil() = delete;
};

#endif
