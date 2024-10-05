#ifndef   FILE_ENC_DEC_UTIL_H
#define   FILE_ENC_DEC_UTIL_H

#include <QByteArray>
#include <QSet>
#include <QList>

#define IDX_ARY_SIZE 4

class FileEncDecUtil
{
public:
    struct collectedCoreData {
        collectedCoreData() 
            : metaCntAry{
                qMakePair(0,0),
                qMakePair(0,0),
                qMakePair(0,0),
                qMakePair(0,0)
            }
            , filecontent()
            , password()
            ///////////////////////////////////
            , headerInfo()
            , metaVec()
            , contentPwdPuzzle_ori()
            , contentPwdPuzzle_enc()
            , contentSz(0)
            , pwdSz(0)
            , randomDummySz(0)
            , needFillHalf(false)
        {
            filecontent.clear();
            password.clear();

            headerInfo.clear();
            metaVec.clear();

            contentPwdPuzzle_ori.clear();
            contentPwdPuzzle_enc.clear();
        }

        // first  : index
        // second : byte value
        QPair<int,char> metaCntAry[IDX_ARY_SIZE];

        QByteArray      filecontent;
        QByteArray      password;

        QByteArray      headerInfo;
        QByteArray      metaVec;
        QByteArray      contentPwdPuzzle_ori;
        QByteArray      contentPwdPuzzle_enc;

        int             contentSz;
        int             pwdSz;
        int             randomDummySz;
        bool            needFillHalf;
    };



    FileEncDecUtil(); 
    virtual ~FileEncDecUtil();

    int setFileContent(const QByteArray& content, int* bIsBinary = nullptr);
    int encFileContent(QByteArray* outBuf, const QByteArray& pwd, FileEncDecUtil::collectedCoreData* pCoreData = nullptr);
    int decFileContent(QByteArray* outBuf, const QByteArray& inputPwd, FileEncDecUtil::collectedCoreData* pCoreData = nullptr);
protected:
    QByteArray generateHeaderInfo(int contentSz, FileEncDecUtil::collectedCoreData* pCoreData = nullptr);
    QByteArray puzzleBa(const QByteArray& ba);
    QByteArray randomInsertPwdByOriginalOrder(const QByteArray& pwd, QList<int>* pOrderList);
    QByteArray randomInsertPuzzledChar(const QByteArray& old,int puzzleCnt, QList<int>* pOrderList);
protected:
    QByteArray m_fileContent;
    // <0   ||   0~31 with out \t \r \n     ,   -128 ~ -1 , 0 ~ 31 ( Exclusive 3 chars , \t \r \n )
    QSet<char>       m_randomChSet;
    QList<char>      m_randomChList;
    QByteArray       m_stripHeaderBuf;

    const int        m_cOffsetCnt;
    const int        m_cSpCnt1;
    const int        m_cSpCnt2;
    const int        m_cUnprintCharCnt;
    const int        m_nBits;
    const int        m_cHeaderLength;

    const int        m_randomContentSzMin;
    const int        m_randomContentSzMax;

    const char       m_chSp1;
    const char       m_chSp2;

    const int        m_compressedLevel;
    const int        m_encdecMethodCnt;

    int              m_metaInfoCnt;
    int              m_nCharMetaBits;
    int              m_fileType;
private:
    int              checkFilecontentValid(bool isCheckHeaderOnly, FileEncDecUtil::collectedCoreData* pCoreData = nullptr, int* pIsNormalBinary = nullptr);
    void             printBufAndMetaList(const QByteArray& buf, QList<int>* pMetaList);
    void             printEncMetaVec(const QByteArray& metaVec);
    void             printDecMetaVec1(const QByteArray& metaVec);
    void             printDecMetaVec2(const QList<QPair<int,int> >& metaVec);


    static const int s_puzzleDummyTag;
    static const int s_contentTag;
    static const int s_passwordTag;

    static const bool s_useRandomSeed;

};


#endif

