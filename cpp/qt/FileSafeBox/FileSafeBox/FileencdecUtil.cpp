#include "FileencdecUtil.h"
#include "EncdecUtil.h"

#include <QtGlobal>
#include <QDebug>
#include <QRandomGenerator> 
#include <QDateTime>
#include <limits> 

/*

------------------------------------------------------------------------------------------------------------------------------------------------------
< Header Info >

    [0 ~ 31] (32 bytes)    : Header Info
       [0 ~ 3]  (4 bytes)  : length offset based on the 4th byte
       [4 ~ 31] (28 bytes) : (2+4+22) 
                              2 special char , '`'  and <Del>     
                              4 values whose index is from [0~3]   which indicate  the encrypted file content's length
                              22 random unprintable bytes

########################################################################################################################################################################################################################################################################################################################################

< Real File Content > :          qCompress(...)    <-->   qUncompress(...)  

Core Core Core :

    vector of meta list ( meta order  contains  the same order of ( the file-content  &&  password  ) 

      xxxx       xxxx    |     xxxx       xxxx    |  ....  .....   
    1st-char   2nd-char  |   3rd-char   4th-char  |

      xx,xx
     enc-method : leader 2 bits =>     00(B): enc/dec 1    01(B): enc/dec 2       10(B): enc/dec 3       11(B): enc/dec 4        ==>     value+1 = enc/dec type
     data-tag   : lower  2 bits =>     0: PuzzleRandomChar      1: File Original Content      2: Password

    Package = ( PWD + Content + PuzzleRandomChar )      ( the 3 parts are ordered with shuffle  ) 



########################################################################################################################################################################################################################################################################################################################################
Encrypt Step
1.  Generate a 32 bytes Header
2.  random insert password into file content ( in the same order )    ( filecontent + password )  (2 parts are shuffled , password with the same order )
3.  random a interger num in the range [ m_randomContentSzMin ~ m_randomContentSzMax )
4.  random insert n bytes into buffer mentioned @ Step2
5.  encrypt the buffer mentioned @ Step4   and  generate a metaVec which indicate the encrpyt method and  content tag
6.  buf6 = qCompress( metaVec + encryptedBuf )
7.  total = Header (in Step-1) + buf6 (in Step-6)



------------------------------------------------------------------------------------------------------------------------------------------------------


*/

namespace testcase {
    static const bool ENABLE_DEBUG = false;
}

//
// static member init
//
const int FileEncDecUtil::s_puzzleDummyTag   = 0;
const int FileEncDecUtil::s_contentTag       = 1;
const int FileEncDecUtil::s_passwordTag      = 2;

const bool FileEncDecUtil::s_useRandomSeed = true;

FileEncDecUtil::FileEncDecUtil()
    : m_fileContent()
    , m_randomChSet()
    , m_randomChList()
    , m_stripHeaderBuf()
    , m_cOffsetCnt(4)
    , m_cSpCnt1(1)
    , m_cSpCnt2(1)
    , m_cUnprintCharCnt(22)
    , m_nBits(8)
    , m_cHeaderLength( 32 )
    , m_randomContentSzMin( 16 )   // inclusive
    , m_randomContentSzMax( 32 )   // exclusive
    , m_chSp1('`')
    , m_chSp2( static_cast<char>(127) )
    , m_compressedLevel( 6 )
    , m_encdecMethodCnt( 4 )
    , m_nCharMetaBits( 4 )
    , m_fileType( 0 )
{
    m_fileContent.clear();
    m_randomChSet.clear();
    m_randomChList.clear();
    m_stripHeaderBuf.clear();

    // 0x80 ~ 0xFF
    for( char ch = 0; ch <= static_cast<char>(31); ++ch ) {
        if ( !( ch == '\t' || ch == '\r' || ch == '\n' ) ) {
            m_randomChSet.insert(ch);
            m_randomChList.push_back( ch );
        }
    }

    for( int chCode = 0x80;  chCode <= 0xFF; ++chCode ) {
        char ch = static_cast<char>(chCode);
        m_randomChSet.insert( ch );
        m_randomChList.push_back( ch );
    }

}


// virtual
FileEncDecUtil::~FileEncDecUtil()
{
    m_fileContent.clear();
}

// Return Value
// 0 :  normal     file
// 1 :  encrypted  file
int FileEncDecUtil::setFileContent(const QByteArray& content)
{
    m_fileContent.clear();
    m_fileContent = content;
    m_metaInfoCnt = 0;

    auto checkHeaderOnly = true;
    auto fileRetType = checkFilecontentValid(checkHeaderOnly);
    m_fileType = fileRetType;
    return fileRetType;
}


QByteArray FileEncDecUtil::puzzleBa(const QByteArray& ba)
{
    QByteArray processedBa(ba);
    QByteArray retBa;
    QRandomGenerator ranFact;
    if ( s_useRandomSeed ) {
        ranFact.seed( static_cast<quint32>( QDateTime::currentSecsSinceEpoch() )  );
    }

    while ( !processedBa.isEmpty() ) {
        int sz = processedBa.size();
        if ( sz > 1 ) {
            int ranIdx = ranFact.bounded(0, sz);
            retBa.push_back( processedBa.at(ranIdx) );
            processedBa.remove( ranIdx, 1);
        } else {
            // sz == 1
            retBa.push_back( processedBa.at(0) );
            processedBa.clear();
        }
    }

    return retBa;

}




QByteArray FileEncDecUtil::generateHeaderInfo(int contentSz, FileEncDecUtil::collectedCoreData* pCoreData /* = nullptr */)
{
    using namespace testcase;

    if ( ENABLE_DEBUG ) {
        qDebug() << "[ENC] : genHeader(...)  metaCnt => " << contentSz;
    }


    QByteArray headerBa;
    QByteArray prefix4;

    // generate   22 + 1 + 1
    QByteArray ranAry;
    int poolSz = m_randomChList.size();
    QRandomGenerator randomFactory;
    if ( s_useRandomSeed ) {
        randomFactory.seed( static_cast<quint32>( QDateTime::currentSecsSinceEpoch() )  );
    }

    for( int i = 0; i < m_cUnprintCharCnt; ++i ) { // 22
        int ranIdx = randomFactory.bounded(0, poolSz);
        ranAry.push_back( m_randomChList.at(ranIdx) );
    }
    ranAry.push_back( m_chSp1 ); // + 1
    ranAry.push_back( m_chSp2 ); // + 1
    // puzzle ranAry
    ranAry = puzzleBa(ranAry);

    // first  : which index to insert 
    // second : byte part of interger 
    QList< QPair<int,char> > orderList;
    
    for( int i = 0 ; i < m_cOffsetCnt; ++i ) {
        int j = 0;
        int rShiftBits = (m_cOffsetCnt - i - 1) * m_nBits;
        char ch = static_cast<char>( ( contentSz >> rShiftBits ) & 0xFF );
        int ranArySz = ranAry.size();
        int insertIdx = randomFactory.bounded(0, ranArySz + 1);
        ranAry.insert( insertIdx, ch);

        if ( i == 0 ) {
            orderList.push_back( qMakePair( insertIdx, ch ) );

            if ( ENABLE_DEBUG ) {
                qDebug() << "[ENC] : metaCnt | " << i << ".  insertPos = " << insertIdx << " value = " << static_cast<int>(ch & 0xFF);
            }
        } else {
            //
            // Core Core Core :
            //      if inserted , the previous index will be changed if necessary

            if ( ENABLE_DEBUG ) {
                qDebug() << "[ENC] : metaCnt | " << i << ".  insertPos = " << insertIdx << " value = " << static_cast<int>(ch & 0xFF);
            }

            // auto whereIdx = -1;
            for( j = 0; j < i; ++j ) {
                if ( insertIdx <= orderList[j].first ) {
                    // if ( whereIdx  == -1 ) {
                    //     whereIdx = j;
                    // }
                    // move to next position
                    orderList[j].first += 1;
                }
            }

            orderList.push_back( qMakePair(insertIdx, ch ) );

        }
    }


    // if ( ENABLE_DEBUG ) {
    //     for( int i = 0; i < orderList.size(); ++i ) {
    //         qDebug() << "[ENC] :  orderList " << i << ".  ( " << orderList[i].first << " , " << static_cast<int>( orderList[i].second & 0xFF) << " )";
    //     }
    // }

    prefix4.reserve( orderList.size() );
    for( int i = 0; i < orderList.size(); ++i ) {
        char byteIndex = static_cast<char>( orderList[i].first );
        prefix4[i] = byteIndex;
        if ( ENABLE_DEBUG ) {
            qDebug() << "[ENC] : prefix4 | " << i << ".  byteIndex = " << static_cast<int>(byteIndex) << " value = " << static_cast<int>( orderList[i].second );
        }

        if ( pCoreData != nullptr ) {
            pCoreData->metaCntAry[i] = qMakePair( orderList[i].first ,  orderList[i].second );
        }
    }


    headerBa = prefix4 + ranAry;
    if ( pCoreData != nullptr ) {
        pCoreData->headerInfo = headerBa;
    }

    return headerBa;

}




int FileEncDecUtil::encFileContent(QByteArray* outBuf, const QByteArray& pwd, FileEncDecUtil::collectedCoreData* pCoreData /* = nullptr */ )
{
    using namespace testcase;

    if ( m_fileType == 1 ) {
        // the file content has already been encrypted
        // no need to encrypt file content
        if ( outBuf != nullptr ) {
            *outBuf = m_fileContent;
        }
        return 0;
    }

    // Do Real <Encrypting>
    int contentSz = m_fileContent.size();
    int pwdSz = pwd.size();
    QRandomGenerator randomFactory;
    if ( s_useRandomSeed ) {
        randomFactory.seed( static_cast<quint32>( QDateTime::currentSecsSinceEpoch() )  );
    }

    int randomPuzzleSz = randomFactory.bounded( m_randomContentSzMin, m_randomContentSzMax );

    int totalContentSz = (contentSz + pwdSz) + randomPuzzleSz;
    int requiredMetaBits = totalContentSz * m_nCharMetaBits;
    bool needfill = (requiredMetaBits % m_nBits != 0);
    int requiredMetaBytes = (requiredMetaBits / m_nBits) + (needfill ?  1 : 0);

    if ( testcase::ENABLE_DEBUG ) {
        qDebug() << "[ENC] contentSz = "          << contentSz;
        qDebug() << "[ENC] pwdSz = "              << pwdSz;
        qDebug() << "[ENC] randomPuzzleSz = "     << randomPuzzleSz;
        qDebug() << "[ENC] needfill = "           << (needfill ? " true " : " false ");
    }

    if ( pCoreData != nullptr ) {
        pCoreData->filecontent = m_fileContent;
        // save real password
        pCoreData->password = pwd;

        pCoreData->contentSz = contentSz;
        pCoreData->pwdSz = pwdSz;
        pCoreData->randomDummySz = randomPuzzleSz;
        pCoreData->needFillHalf = needfill;
    }

    QList<int> metaList;
    metaList.reserve( contentSz );
    for( int i = 0; i < contentSz; ++i ) {
        metaList.push_back( s_contentTag );
    }

    auto headerInfo = generateHeaderInfo( totalContentSz, pCoreData );


    auto contentWithPwd = randomInsertPwdByOriginalOrder(pwd, &metaList);
    printBufAndMetaList( contentWithPwd, &metaList);
    auto contentWithPwd_AndPuzzle = randomInsertPuzzledChar(contentWithPwd, randomPuzzleSz, &metaList);
    printBufAndMetaList( contentWithPwd_AndPuzzle, &metaList);

    if ( pCoreData != nullptr ) {
        pCoreData->contentPwdPuzzle_ori = contentWithPwd_AndPuzzle;
    }
    
    QString strline = QStringLiteral("  %1 @ %2").arg( __FILE__ ).arg( __LINE__ );
    QString strWhat("2 container's size are not equal ( puzzle.size() != meta.size() ).");
    Q_ASSERT_X(contentWithPwd_AndPuzzle.size() == metaList.size(), strline.toUtf8().constData(), strWhat.toUtf8().constData() );
    
    QByteArray metaAry;
    char metaInfo = 0;
    int encTypeId = 2;
    int contentWithPwdAndPuzzleCnt = contentWithPwd_AndPuzzle.size();
    for( int i = 0; i < contentWithPwdAndPuzzleCnt; ++i ) {
        // encTypeId = randomFactory.bounded(0, m_encdecMethodCnt) + 1;
        contentWithPwd_AndPuzzle[i] = EncryptDecryptUtil::enc1ch(encTypeId, contentWithPwd_AndPuzzle[i] );

        int tag = metaList.at(i);
        unsigned int half = static_cast<unsigned int>( ( (encTypeId-1) << 2) | tag );
        if ( i % 2 == 0 ) {
            metaInfo = static_cast<char>(half);

            if ( testcase::ENABLE_DEBUG ) {
                QString sNum;
                sNum.setNum(metaInfo, 16);
                sNum = sNum.toUpper();
                qDebug() << "[ENC] " << (i+1) << ". " << "tag = " << tag << ",  metaInfo = "  << sNum;
            }
        } else {
            // to generate a 8-bit char
            metaInfo = static_cast<char>( ( metaInfo << 4 ) | static_cast<int>(half) );
            metaAry.push_back(metaInfo);

            if ( testcase::ENABLE_DEBUG ) {
                QString sNum;
                sNum.setNum(metaInfo, 16);
                sNum = sNum.toUpper();
                qDebug() << "[ENC] " << (i+1) << ". " << "tag = " << tag << ",  push metaInfo = "  << sNum;
            }
        }
    }
    
    auto bFillHalf = ( (contentWithPwdAndPuzzleCnt % 2) == 1 );
    if ( bFillHalf ) {
        metaInfo = static_cast<char>( ( metaInfo << 4 ) & 0xF0 );
        metaAry.push_back(metaInfo);


        if ( testcase::ENABLE_DEBUG ) {
            QString sNum;
            sNum.setNum(metaInfo, 16);
            sNum = sNum.toUpper();
            qDebug() << "[ENC] " << "Final , fill last meta : " << ". " << " push metaInfo = "  << sNum;
        }
    }
    printEncMetaVec(metaAry);

    if ( pCoreData != nullptr ) {
        pCoreData->metaVec = metaAry;
        pCoreData->contentPwdPuzzle_enc = contentWithPwd_AndPuzzle;
    }


    strline = QStringLiteral("  %1 @ %2").arg( __FILE__ ).arg( __LINE__ );
    strWhat = "2 container's size are not equal ( metaAry.size() != requiredMetaBytes ).";
    Q_ASSERT_X( metaAry.size() == requiredMetaBytes , strline.toUtf8().constData(), strWhat.toUtf8().constData() );

    QByteArray realContentBa = metaAry + contentWithPwd_AndPuzzle;
    QByteArray compressedContent = qCompress( realContentBa, m_compressedLevel); // Core Core Core : Do Compress
    if ( outBuf != nullptr ) { 
        *outBuf = (headerInfo + compressedContent);
    }
    return 1;
}





QByteArray FileEncDecUtil::randomInsertPwdByOriginalOrder(const QByteArray& pwd, QList<int>* pOrderList)
{
    QByteArray retBuf(m_fileContent);

    int lastInsertIdx = 0;
    int ranIdx = 0;
    QRandomGenerator ranFact;
    if ( s_useRandomSeed ) {
        ranFact.seed( static_cast<quint32>( QDateTime::currentSecsSinceEpoch() )  );
    }

    for( int i = 0; i < pwd.size(); ++i )
    {
        auto ch = pwd.at(i);
        int bufSz = retBuf.size();
        if ( i == 0 ) {
            ranIdx = ranFact.bounded(0, bufSz);
            if ( ranIdx == 0 ) {
                retBuf.push_front(ch);
                pOrderList->push_front( s_passwordTag );
            } else {
                retBuf.insert(ranIdx, ch);
                pOrderList->insert(ranIdx, s_passwordTag );
            }
            lastInsertIdx = ranIdx;
        } else {
            // i >= 1 , make sure the next password's ch must insert after the previous's ch
            int begIdx = lastInsertIdx + 1;
            ranIdx = ranFact.bounded(begIdx, bufSz);
            retBuf.insert(ranIdx , ch);
            pOrderList->insert(ranIdx, s_passwordTag);
            lastInsertIdx = ranIdx;
        }
    }

    return retBuf;
}


QByteArray FileEncDecUtil::randomInsertPuzzledChar(const QByteArray& old,int puzzleCnt, QList<int>* pOrderList)
{
    QByteArray retBuf(old);

    QRandomGenerator ranIdxFact;
    QRandomGenerator ranchFact;
    if ( s_useRandomSeed ) {
        auto seed1 = static_cast<quint32>( QDateTime::currentSecsSinceEpoch() );
        auto seed2 = seed1 % 1234u;
        ranIdxFact.seed( seed1 );
        ranchFact.seed ( seed2 );
    }

    int poolSz = m_randomChList.size();
    do {
        int bufSz = retBuf.size();
        int insertIdx = ranIdxFact.bounded(0, bufSz + 1);
        int chIdx = ranchFact.bounded(0, poolSz);

        auto insertCh = m_randomChList.at(chIdx);

        if ( insertIdx == 0 ) {
            retBuf.push_front( insertCh );
            pOrderList->push_front( s_puzzleDummyTag );
        } else if ( insertIdx == bufSz ) {
            retBuf.push_back( insertCh );
            pOrderList->push_back( s_puzzleDummyTag );
        } else {
            retBuf.insert( insertIdx, insertCh );
            pOrderList->insert(insertIdx, s_puzzleDummyTag );
        }
    } while ( --puzzleCnt > 0 );

    return retBuf;
}




int FileEncDecUtil::decFileContent(QByteArray* outBuf, const QByteArray& inputPwd, FileEncDecUtil::collectedCoreData* pCoreData /* = nullptr */ )
{
    using namespace testcase;

    if ( m_fileType == 0 ) {
        // the file content has already been decrypted
        // no need to encrypt file content
        if ( outBuf != nullptr ) {
            *outBuf = m_fileContent;
        }

        if ( ENABLE_DEBUG ) {
            qDebug() << "[DEC] : File Content is normal , no need to decrypt ";
        }

        return 0;
    }

    //
    // Do Real <Decrypting>
    //
    // m_stripHeaderBuf   is uncompressed already
    auto checkHeaderOnly = false;
    auto fileType = checkFilecontentValid(checkHeaderOnly, pCoreData);
    if ( fileType != 1 ) {
        if ( ENABLE_DEBUG ) {
            qDebug() << "[DEC] : Checked ! File Content is normal , no need to decrypt ";
        }
        return 3;
    }

    
    int requiredMetaBits   = m_metaInfoCnt * m_nCharMetaBits;
    bool needFill = (requiredMetaBits % m_nBits != 0);
    int requiredMetaBytes  = (requiredMetaBits  / m_nBits) + ( needFill ?  1 : 0);

    if( (requiredMetaBytes + m_metaInfoCnt) != m_stripHeaderBuf.size()  ) {
        if ( ENABLE_DEBUG ) {
            qDebug() << "[DEC] :  (requiredMetaBytes + m_metaInfoCnt) != m_stripHeaderBuf.size()  " << m_metaInfoCnt;
        }
        return 3;
    }

    if ( ENABLE_DEBUG ) {
        qDebug() << "[DEC] : m_metaInfoCnt = " << m_metaInfoCnt;
        qDebug() << "[DEC] : requiredMetaBytes = " << requiredMetaBytes;
    }


    if ( pCoreData != nullptr ) {
        pCoreData->needFillHalf = needFill;
    }

    // first  : enc/dec method type
    // second : data tag
    QByteArray decMetaVec;
    QList< QPair<int,int> > metaList;
    for( int i = 0; i < requiredMetaBytes; ++i ) {
        char metaCh = m_stripHeaderBuf.at(i);
        decMetaVec.push_back( metaCh );
        int higher4bits = static_cast<int>( (metaCh >> 4) & 0xF );
        int higherEncMethod = ( (higher4bits >> 2) & 0x3) + 1;
        int higherTag       = (  higher4bits  & 0x3);

        int lower4bits  = static_cast<int>(  metaCh       & 0xF );
        int lowerEncMethod = ( (lower4bits >> 2) & 0x3) + 1;
        int lowerTag       = (  lower4bits       & 0x3);

        if ( i != (requiredMetaBytes-1) ) {
            metaList.push_back( qMakePair( higherEncMethod, higherTag) );
            metaList.push_back( qMakePair( lowerEncMethod, lowerTag) );
        } else {
            // last meta byte
            if ( needFill ) {
                // only push higher 4 bits , skip the lower4bits
                metaList.push_back( qMakePair( higherEncMethod, higherTag) );
            } else {
                metaList.push_back( qMakePair( higherEncMethod, higherTag) );
                metaList.push_back( qMakePair( lowerEncMethod, lowerTag) );
            }
        }
    }
    printDecMetaVec1(decMetaVec);
    printDecMetaVec2(metaList);
    if ( pCoreData != nullptr ) {
        pCoreData->metaVec = decMetaVec;
    }


    QByteArray realContentWithPwd_DummyBuf = m_stripHeaderBuf.mid(requiredMetaBytes);
    if ( pCoreData != nullptr ) {
        pCoreData->contentPwdPuzzle_enc = realContentWithPwd_DummyBuf;
    }

    if ( metaList.size() != realContentWithPwd_DummyBuf.size() ) {
        if ( ENABLE_DEBUG ) {
            qDebug() << "[DEC] : metaList.size() != realContentWithPwd_DummyBuf.size() ";
        }
        return 3;
    }


    QByteArray randomBuf;
    QByteArray realContent;
    QByteArray realPwd;
    QByteArray restoreBuf;
    for ( auto i = 0; i < metaList.size(); ++i ) {
        auto pr = metaList.at(i);
        auto encMethod = pr.first;
        auto chTag = pr.second;

        auto ch = realContentWithPwd_DummyBuf[i];
        auto decCh = EncryptDecryptUtil::dec1ch(encMethod , ch );

        if ( ENABLE_DEBUG ) {
            QString encedCh;
            encedCh.setNum( static_cast<int>(ch & 0xFF), 16);
            encedCh = encedCh.toUpper();

            QString decedCh;
            decedCh.setNum( static_cast<int>(decCh & 0xFF), 16);
            decedCh = decedCh.toUpper();

            qDebug() << "[DEC] : " << (i+1) << " tag = " << chTag << " encid = " << encMethod <<  ". 0x" << encedCh << " <-> 0x" << decedCh;
        }


        if ( chTag == s_puzzleDummyTag ) { // random dummy ch
            randomBuf.push_back( decCh );
        } else if ( chTag == s_contentTag ) { // real content ch
            realContent.push_back( decCh );
        } else if ( chTag == s_passwordTag ) { // real password ch
            realPwd.push_back( decCh );
        }

        restoreBuf.push_back( decCh );
    }

    if ( pCoreData != nullptr ) {
        pCoreData->filecontent = realContent;
        pCoreData->password = realPwd;
        pCoreData->contentPwdPuzzle_ori = restoreBuf;

        pCoreData->contentSz = realContent.size();
        pCoreData->pwdSz = realPwd.size();
        pCoreData->randomDummySz = randomBuf.size();
    }


    if ( realPwd != inputPwd ) {
        if ( ENABLE_DEBUG ) {
            qDebug() << "[DEC] : Password is incorrected. decPassword = " << realPwd << " , inputPwd = " << inputPwd;
        }
        return 2; // password is incorrect
    }

    if ( outBuf != nullptr ) {
        *outBuf = realContent;
    }

    return 1;
}


int   FileEncDecUtil::checkFilecontentValid(bool isCheckHeaderOnly, FileEncDecUtil::collectedCoreData* pCoreData /* = nullptr */)
{
    using namespace testcase;

    int retFileType = 0; // 0: normal file type

    int bufSz = m_fileContent.size();
    if ( bufSz > m_cHeaderLength ) {
        auto offset0 = static_cast<int>( m_fileContent[0] );
        auto offset1 = static_cast<int>( m_fileContent[1] );
        auto offset2 = static_cast<int>( m_fileContent[2] );
        auto offset3 = static_cast<int>( m_fileContent[3] );

        auto isValid = true;
        auto metaDataCnt = 0;

        auto offsetMatchedCnt = 0;
        auto sp1Cnt = 0;
        auto sp2Cnt = 0;
        auto unprintCnt = 0;
        // [4,32)
        for( auto idx = m_cOffsetCnt; idx < m_cHeaderLength; ++idx )
        {
            char ch = m_fileContent[idx];
            unsigned int code = static_cast<unsigned int>( ch & 0xFF );
            auto realOffset = idx - m_cOffsetCnt;
            if ( realOffset == offset0 ) {
                metaDataCnt |= ( (code << 24) & 0xFF000000);
                ++offsetMatchedCnt;

                if ( pCoreData != nullptr ) {
                    pCoreData->metaCntAry[0] = qMakePair(offset0,  ch);
                }
            } else if ( realOffset == offset1 ) {
                metaDataCnt |= ( (code << 16) & 0x00FF0000);
                ++offsetMatchedCnt;

                if ( pCoreData != nullptr ) {
                    pCoreData->metaCntAry[1] = qMakePair(offset1,  ch);
                }
            } else if ( realOffset == offset2 ) {
                metaDataCnt |= ( (code << 8) & 0x0000FF00);
                ++offsetMatchedCnt;

                if ( pCoreData != nullptr ) {
                    pCoreData->metaCntAry[2] = qMakePair(offset2,  ch);
                }
            } else if ( realOffset == offset3 ) {
                metaDataCnt |= ( code & 0x000000FF );
                ++offsetMatchedCnt;

                if ( pCoreData != nullptr ) {
                    pCoreData->metaCntAry[3] = qMakePair(offset3,  ch);
                }
            } else {
                if ( ch == m_chSp1 ) {
                    ++sp1Cnt;
                } else if ( ch == m_chSp2 ) {
                    ++sp2Cnt;
                } else {
                    if ( m_randomChSet.find(ch) != m_randomChSet.end() ) {
                        ++unprintCnt;
                    } else {
                        isValid = false;
                        break;
                    }
                }
            }
        }

        if ( isValid ) {
            if (   sp1Cnt == m_cSpCnt1 
                && sp2Cnt == m_cSpCnt2  
                && unprintCnt == m_cUnprintCharCnt
                && offsetMatchedCnt == m_cOffsetCnt ) 
            {
                if ( ENABLE_DEBUG ) {
                    qDebug() << "[DEC] :" 
                             << "  0 : " << static_cast<int>( offset0 )
                             << ", 1 : " << static_cast<int>( offset1 )
                             << ", 2 : " << static_cast<int>( offset2 )
                             << ", 3 : " << static_cast<int>( offset3 );
                }


                if ( isCheckHeaderOnly ) {
                    retFileType = 1; // 1: enc file type
                } else {
                    if ( pCoreData != nullptr ) {
                        pCoreData->headerInfo = m_fileContent.mid(0, m_cHeaderLength);
                    }

                    // File's Header  32 byte are all valid
                    QByteArray restBuf = qUncompress( m_fileContent.mid(m_cHeaderLength) );
                    int requiredMetaBits   = metaDataCnt * m_nCharMetaBits;
                    int requiredMetaBytes  = (requiredMetaBits  / m_nBits) + ( (requiredMetaBits % m_nBits != 0) ?  1 : 0);

                    if ( (requiredMetaBytes + metaDataCnt) == restBuf.size() ) {
                        retFileType = 1;  // 1: enc file type
                        m_metaInfoCnt = metaDataCnt;
                        m_stripHeaderBuf = restBuf;
                    }
                }
            } 
        }
    }

    return retFileType;
}



void   FileEncDecUtil::printBufAndMetaList(const QByteArray& buf, QList<int>* pMetaList)
{
    if ( testcase::ENABLE_DEBUG ) {
        if ( buf.size() == pMetaList->size() ) {
            QString displayChar;
            QString tagName;
            qDebug() << "====================================================================================================";
            for( int i = 0; i < buf.size(); ++i ) 
            {
                int tag = pMetaList->at(i);
                char ch = buf[i];
                int code = static_cast<int>(ch & 0xFF);
                if ( code < 32 ) {
                    if ( code == 9 ) {
                        displayChar = "'\\t'";
                    } else if ( code == 10 ) {
                        displayChar = "'\\n'";
                    } else if ( code == 13 ) {
                        displayChar = "'\\r'";
                    } else {
                        displayChar = "'?'";
                    }
                } else if ( code == 32 ) {
                    displayChar = "' '";
                } else {
                    if ( code < 127 ) {
                        displayChar = QString("'%1'").arg( ch );
                    } else {
                        displayChar = "'?'";
                    }
                }


                if ( tag == s_puzzleDummyTag ) {
                    tagName = "PuzzleDummy";
                } else if (  tag == s_contentTag ) {
                    tagName = "FileContent";
                } else if (  tag == s_passwordTag ) {
                    tagName = "Password";
                } else {
                    tagName = "Unknown Type ";
                }
                qDebug() << (i+1) << ". " << displayChar << "  => " << tagName;
            }
            qDebug() << "===================================================================================================="
                     << endl;
        }
    }
}


void   FileEncDecUtil::printEncMetaVec(const QByteArray& metaVec)
{
    if ( testcase::ENABLE_DEBUG ) {
        qDebug() << "====================================================================================================";
        for( auto i = 0; i < metaVec.size(); ++i )
        {
            auto ch = metaVec[i];
            int  code = static_cast<int>( ch & 0xFF);
            QString hexCode;
            hexCode.setNum( code, 16 );
            hexCode = hexCode.toUpper();

            qDebug() << "[ENC] " << (i+1) << ". 0x" << hexCode;
        }

        qDebug() << "===================================================================================================="
                 << endl;
    }
}


void   FileEncDecUtil::printDecMetaVec1(const QByteArray& metaVec)
{

    if ( testcase::ENABLE_DEBUG ) {
        qDebug() << "====================================================================================================";
        for( auto i = 0; i < metaVec.size(); ++i )
        {
            auto ch = metaVec[i];
            int  code = static_cast<int>( ch & 0xFF);
            QString hexCode;
            hexCode.setNum( code, 16 );
            hexCode = hexCode.toUpper();

            qDebug() << "[DEC] " << (i+1) << ". 0x" << hexCode;
        }

        qDebug() << "===================================================================================================="
                 << endl;
    }

}


void   FileEncDecUtil::printDecMetaVec2(const QList<QPair<int,int> >& prVec)
{
    if ( testcase::ENABLE_DEBUG ) {
        qDebug() << "====================================================================================================";
        for( auto i = 0; i < prVec.size(); ++i )
        {
            auto pr = prVec.at(i);
            qDebug() << "[DEC] " << (i+1) << ". methodId = " << pr.first << ", tag = " << pr.second;
        }
        qDebug() << "===================================================================================================="
                 << endl;
    }
}



