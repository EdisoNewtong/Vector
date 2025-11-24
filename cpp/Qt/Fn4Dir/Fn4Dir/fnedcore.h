#ifndef FNEDCORE_H
#define FNEDCORE_H

#include <QObject>
#include <QVector>
#include <QFile>

#define SPECIAL_ARY_SZ     4
#define SPECIAL_ELEMENTS   5
#define DUMMY_ARY_SIZE    30
#define WITHOUT_3_ARY_SZ   4

#define TO_ENC_ARGS(arg) ( arg.additionalExp | (arg.specialCharIdx<<3) | (arg.nShiftBits<<6) | (arg.bShiftDir<<7) | (arg.bBigOrLittle<<8) )

struct SpecialChars
{
    char chars[SPECIAL_ARY_SZ];
    int len;
};

struct EncArgWrap
{
    int additionalExp;    // [4,6]
    int specialCharIdx;   // [0,5)
    int nShiftBits;       // [1,8)
    int bShiftDir;       //  0(<<) or 1(>>)
    int bBigOrLittle;    //  0:Big Endian    1:Little Endian
};


class FNEDCore : public QObject
{
    Q_OBJECT
public:
    explicit FNEDCore(QObject *parent = nullptr);

    bool encryptFile(const QString& fileAbsPath,  QString& errorMsg);
    bool encryptFile_withArg(const QString& fileAbsPath, int arg, QString& errorMsg);

    bool decryptFile(const QString& fileAbsPath, QString& newFileName, QString& errorMsg);

    bool encryptFile_TestAll(const QString& fileAbsPath,  QString& errorMsg);
    bool decryptFile_TestAll_WithCmp(const QString& fileAbsPath,  QString& errorMsg);

    // util
    static bool is2FileEqual(const QString& fname1, const QString& fname2, QString& errorMsg);
    static int  genIDByArgs(const EncArgWrap& obj);
signals:

public slots:

protected:
    // Util
    bool genNewFileName(const QString& fileAbsPath,  QString& newName, int givenArg, bool forEncOrDec, QString& errorMsg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Write Part
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool writeHead3Bytes(QFile& encFile, int additionalExponentFlag, int& ranCharIdx);
    bool writeNextDummyBytes(QFile& encFile, int additionalExponentFlag);
    bool writeCore10Bytes(QFile& encFile, qint64 originalFileSz, int nShiftBits, bool bIsLittleEndian);

    bool writeOriginalEncData(QFile& encFile, QFile& originalFile, qint64 originalFileSz, int additionalExponentFlag, int nShiftBits,bool bRightShiftFlag, QString& errorMsg);
     bool writeOriginalEncData_LargeFile(QFile& encFile, QFile& originalFile, qint64 originalFileSz, int nShiftBits,bool bRightShiftFlag,  qint64 base, QString& errorMsg);
     bool writeOriginalEncData_NormalFile(QFile& encFile, QFile& originalFile, qint64 originalFileSz, int nShiftBits,bool bRightShiftFlag, qint64 base, QString& errorMsg);

    bool writeTailDummyBytes(QFile& encFile, int additionalExponentFlag);
    bool writeTailKeyBytes(QFile& encFile, int additionalExponentFlag, int ranCharIdx, bool bIsLittleEndian, bool bRightShiftFlag);


    bool writeEncData(QFile& originalFileObj, qint64 originalFileSz, const QString& newFileName, QString& errorMsg);
     bool writeEncDataWithArgs(QFile& originalFileObj, qint64 originalFileSz, const QString& newFileName, int buildArgs, QString& errorMsg);


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read Part
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool readHead3Bytes(QFile& file2read, int& additionalExponentFlag, char* specialCharAry, QString& errorMsg);
    bool skipReadDummyBytes(QFile& file2read, int additionalExponentFlag, QString& errorMsg);
    bool readCore10Bytes(QFile& file2read,  int additionalExponentFlag, char* core10Ary, bool bIsBigEndian, qint64& originalSz, qint64 read2fileOriginalSz, unsigned int& nShiftBits, QString& errorMsg);

    bool readTailKeyBytes(QFile& file2read, qint64 illFileSize,int additionalExponentFlag,char* spChAry, bool& bIsBigEndian, bool& bRightShiftFlag,  QString& errorMsg);
    bool readEncDataAndThenWrite(QFile& file2read, const QString& newFileName, qint64 originalFileSz, int additionalExponentFlag, bool bRightShiftFlag, unsigned int nShiftBits, QString& errorMsg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Restore to original file content part
    ////////////////////////////////////////////////////////////////////////////////////////////////////
     bool readEncDataAndThenWrite4NormalFile(QFile& file2read, QFile& wrtFile, qint64 originalFileSz, bool bRightShiftFlag,unsigned int nShiftBitCnt,  QString& errorMsg );
     bool readEncDataAndThenWrite4LargeFile(QFile& file2read, QFile& wrtFile, qint64 baseIdx, qint64 originalFileSz, bool bRightShiftFlag, unsigned int nShiftBitCnt, QString& errorMsg );




    static void bitShift1(char& left, char& right, bool bShiftRightFlag, unsigned int nShiftBits );
    static void bitShift2(char& left, char& right, bool bShiftRightFlag, unsigned int nShiftBits );

    static void bitShift1Char_1(char& ch,  bool bShiftRightFlag, unsigned int nShiftBits );
    static void bitShift1Char_2(char& ch,  bool bShiftRightFlag, unsigned int nShiftBits );

    static int howMany1InChar(char ch);
    static int getNBit(char ch, int idx);

    static QString changeBaseName(const QString& fileNameOnly, const QString& suffix, bool bIsEnc);
    static QString changeSuffixName(const QString& fileNameOnly, bool bIsEnc, int* bIsMatched);

    static const unsigned int sc_bitsInByte;
    static const unsigned int sc_bitsInShort;
    static const          int sc_maxint;


    static const char           s_randomDummyCharAry[DUMMY_ARY_SIZE];
    static const SpecialChars   s_special_ary[SPECIAL_ELEMENTS];
    static const QString        s_special_str_ary[SPECIAL_ELEMENTS];
    static const int            s_aryWithout3[WITHOUT_3_ARY_SZ];

    static const QVector< QVector<int> > s_1Cnt_ary;
    static const QVector< QVector<int> > s_nth1Cnt_ary;

    static const QVector< QVector<int> > s_nth0Cnt_ary;


    static const qint64 sc_ll_threshold;
    static const qint64 sc_ll_cmpBlockSize;

    ///////////////////////////////////////////////////////////////////////////////////
    // Debug Use
    static const QVector<QString> s_debugLR;
    static const QVector<QString> s_debugBigLittle;

    static const QString sc_encTest_Suffix;
    static       bool sc_bUseNameChangeFunc;

};

#endif // FNEDCORE_H
