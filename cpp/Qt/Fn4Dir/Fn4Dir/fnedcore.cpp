#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRandomGenerator> 
#include <QSet>
#include <QMap>
#include <QByteArray>
#include <QPair>

#include "fnedcore.h"



// static 
const unsigned int FNEDCore::sc_bitsInByte  = 8;
const unsigned int FNEDCore::sc_bitsInShort = 16;
const          int FNEDCore::sc_maxint      = 0x7FFFFFFF;

const qint64 FNEDCore::sc_ll_threshold    = 4096ll; // 4 KB
const qint64 FNEDCore::sc_ll_cmpBlockSize = 4096ll; // 4 KB

// static 
const char           FNEDCore::s_randomDummyCharAry[DUMMY_ARY_SIZE] { 0,1,2,3,4,5,6,7,8,    11,12,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31, 127 };


// static
const SpecialChars   FNEDCore::s_special_ary[SPECIAL_ELEMENTS] {
    {  { static_cast<char>(0xF0), static_cast<char>(0x9D),  static_cast<char>(0x84), static_cast<char>(0x9E) }, 4 },   // 𝄞
    {  { static_cast<char>(0xF0), static_cast<char>(0x9D),  static_cast<char>(0x84), static_cast<char>(0xA2) }, 4 },   // 𝄢
    {  { static_cast<char>(0xF0), static_cast<char>(0x9F),  static_cast<char>(0x94), static_cast<char>(0x92) }, 4 },   // 🔒
    {  { static_cast<char>(0xE3), static_cast<char>(0x80),  static_cast<char>(0xA0), static_cast<char>(0x00) }, 3 },   // 〠   [3]
    {  { static_cast<char>(0xF0), static_cast<char>(0x9F),  static_cast<char>(0x9B), static_cast<char>(0xAA) }, 4 }    // 🛪
};

    
// static 
const QString        FNEDCore::s_special_str_ary[SPECIAL_ELEMENTS] {
    QStringLiteral("𝄞"),
    QStringLiteral("𝄢"),
    QStringLiteral("🔒"),
    QStringLiteral("〠"),
    QStringLiteral("🛪")
};


// static 
const int            FNEDCore::s_aryWithout3[WITHOUT_3_ARY_SZ] { 0,1,2, /* 3 */   4 };


// static
const QVector< QVector<int> > FNEDCore::s_1Cnt_ary {
    QVector<int>({  1, 2, 4, 8, 16  }),                     //  [0] 5 possibilities   one   1    inside the array's element 
    QVector<int>({  3, 5, 6, 12, 17, 18, 20, 24  }),        //  [1] 8 possibilities   two   1(s) inside the array's element 
    QVector<int>({  7, 11, 14, 19, 21, 22, 25, 26, 28  }),  //  [2] 9 possibilities   three 1(s) inside the array's element 
    QVector<int>({ 15, 23, 27, 29, 30 }),                   //  [3] 5 possibilities   four  1(s) inside the array's element 
    QVector<int>({ 31 }),                                   //  [4] 1 possibilities   five  1(s) inside the array's element 
};

// static
const QVector< QVector<int> > FNEDCore::s_nth1Cnt_ary {
    QVector<int>({  1, 3, 5,    7, 11, 15, 17, 19, 21, 23, 25, 27, 29, 31,  }),          // The 0th is 1,     14 possibilities
    QVector<int>({  2, 3, 6,    7, 11, 14, 15, 18, 19, 22, 23, 26, 27, 30, 31,  }),      // The 1st is 1,     15 possibilities
    QVector<int>({  4, 5, 6,    7, 12, 14, 15, 20, 21, 22, 23, 28, 29, 30, 31,  }),      // The 2nd is 1,     15 possibilities
    QVector<int>({  8, 11, 12, 14, 15, 24, 25, 26, 27, 28, 29, 30, 31     }),            // The 3rd is 1,     13 possibilities
    QVector<int>({  16, 17,18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  }),  // The 4th is 1,     16 possibilities
};


// static 
const QVector< QVector<int> > FNEDCore::s_nth0Cnt_ary {
    QVector<int>({ 0, 1, 2, 3, 8, 11, 16, 17, 18, 19, 24, 25, 26, 27 }), // 2nd with 0: 14 counts
    QVector<int>({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 14, 15 }),         // 4th with 0: 13 counts
};


const QVector<QString> FNEDCore::s_debugLR        { QStringLiteral("L"), QStringLiteral("R") };
const QVector<QString> FNEDCore::s_debugBigLittle { QStringLiteral("BigEnd"), QStringLiteral("LittleEnd") };

    
const QString FNEDCore::sc_encTest_Suffix("encTest");
bool FNEDCore::sc_bUseNameChangeFunc = true;



//////////////////////////////////////////////////
// Function Encrypt/Decrypt   Core
// ^      ^ ^       ^         ^^^^
FNEDCore::FNEDCore(QObject *parent) : QObject(parent)
{
    /*
    QString s1("🔒");
    QByteArray ba;
    ba.append( char('\xF0') );
    ba.append( char('\x9F') );
    ba.append( char('\x94') );
    ba.append( char('\x92') );
    QString s2(ba); // ("\xF0\x9F\x94\x92");

    if ( s1 == s2 ) {
        qDebug() << "s1 == s2. ";
    } else {
        qDebug() << "s1 != s2. ";
    }
    */

}


bool FNEDCore::genNewFileName(const QString& fileAbsPath,  QString& newName, int givenArgs, bool forEncOrDec, QString& errorMsg)
{
    static const QString sc_encSuffix("enc");
    static const QString sc_decSuffix("dec");
    // true: enc     false:dec
    const QString realSuffix = forEncOrDec ? sc_encSuffix : sc_decSuffix;

    QFileInfo fInfo(fileAbsPath);
    QFile originalFile( fileAbsPath );
    if ( !originalFile.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }

    auto bNoSuffixFlag = fInfo.suffix().isEmpty();
    if ( bNoSuffixFlag ) {
        if ( givenArgs == -1 ) {
            newName = QString("%1/%2_%3").arg( fInfo.absolutePath() ).arg( FNEDCore::changeBaseName(fInfo.baseName(), "", forEncOrDec ) ).arg( realSuffix );
        } else {
            newName = QString("%1/%2_%3-%4").arg( fInfo.absolutePath() ).arg( FNEDCore::changeBaseName(fInfo.baseName(), "", forEncOrDec) ).arg( realSuffix ).arg( QString("Arg%1").arg(givenArgs) );
        }
    } else {
        // originalFileName_enc.???
        int iMatchFlag = 0;
        if ( givenArgs == -1 ) {
            newName = QString("%1/%2_%3.%4")
                            .arg( fInfo.absolutePath() )
                            .arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec ) )
                            .arg(  realSuffix   )
                            .arg( FNEDCore::changeSuffixName( fInfo.suffix(), forEncOrDec, &iMatchFlag)  );
        } else {
            newName = QString("%1/%2_%3-%4.%5")
                            .arg( fInfo.absolutePath() )
                            .arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec ) )
                            .arg(  realSuffix   )
                            .arg(  QString("Arg%1").arg(givenArgs)  )
                            .arg( FNEDCore::changeSuffixName( fInfo.suffix(), forEncOrDec, &iMatchFlag)  );
        }
    }

    QFileInfo tmpfile(  newName );
    if ( tmpfile.exists() ) {
        int i = 1;
        while( 1 ) {
            if ( bNoSuffixFlag ) {
                // absDir/originalFileName_enc_1
                if ( givenArgs == -1 ) {
                    newName = QString("%1/%2_%3-%4").arg( fInfo.absolutePath() ).arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec) ).arg( realSuffix ).arg(i);
                } else {
                    newName = QString("%1/%2_%3-%4-%5").arg( fInfo.absolutePath() ).arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec) ).arg( realSuffix ).arg( QString("Arg%1").arg(givenArgs) ).arg(i);
                }
            } else {
                // absDir/originalFileName_enc_1.???
                int iMatchFlag = 0;
                if ( givenArgs == -1 ) {
                    newName = QString("%1/%2_%3_%4.%5")
                            .arg( fInfo.absolutePath() )
                            .arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec) )
                            .arg(  realSuffix   )
                            .arg( i )
                            .arg( FNEDCore::changeSuffixName( fInfo.suffix(), forEncOrDec, &iMatchFlag)  );
                } else {
                    newName = QString("%1/%2_%3-%4_%5.%6")
                            .arg( fInfo.absolutePath() )
                            .arg( FNEDCore::changeBaseName( fInfo.baseName(), fInfo.suffix(), forEncOrDec ) )
                            .arg(  realSuffix   )
                            .arg(  QString("Arg%1").arg(givenArgs)   )
                            .arg( i )
                            .arg( FNEDCore::changeSuffixName(fInfo.suffix(), forEncOrDec, &iMatchFlag)  );
                }
            }

            QFileInfo tmpfile_num(  newName );
            if( !tmpfile_num.exists() ) {
                break;
            }

            ++i;
        }
    }

    // qDebug() << QString("Generate new [%1] file with name %2").arg( realSuffix.toUpper() ).arg( newName );

    return true;
}



/******************************************************************************************************

     qDebug() << "fileAbsPath = " << fileAbsPath;
     qDebug() << "absoluteDir = " << fInfo.absoluteDir().path();
     qDebug() << "absoluteFilePath = " << fInfo.absoluteFilePath();
     qDebug() << "absolutePath = " << fInfo.absolutePath();
     qDebug() << "baseName = " << fInfo.baseName();
     qDebug() << "canonicalFilePath = " << fInfo.canonicalFilePath();
     qDebug() << "canonicalPath = " << fInfo.canonicalPath();
     qDebug() << "completeBaseName = " << fInfo.completeBaseName();
     qDebug() << "dir = " << fInfo.dir().path();
     qDebug() << "fileName = " << fInfo.fileName();
     qDebug() << "filePath = " << fInfo.filePath();
     qDebug() << "suffix = " << fInfo.suffix();

 
    absolutePath     =  "D:/Mavic3Pro/2025-09-07"
    baseName         =  "DJI_20250907074601_0032_D"
    fileName         =  "DJI_20250907074601_0032_D.MP4"
    suffix()

******************************************************************************************************/
bool FNEDCore::encryptFile(const QString& fileAbsPath, QString& errorMsg)
{
    QString newName;
    auto bHasNewFileNameFlag = genNewFileName(fileAbsPath, newName, -1, true, errorMsg);
    if ( !bHasNewFileNameFlag ) {
        qDebug() << "generate new file name failed :  " << errorMsg;
        return false;
    }


    //
    // New file name is not exists , create a new file
    //
    // get the real file size in bytes
    QFile originalFile( fileAbsPath );
    if ( !originalFile.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }
    qint64 illFileSize = originalFile.size();
    if ( illFileSize <= 1 ) {
        errorMsg = QString("File size is %1, Do Nothing. ").arg( illFileSize );
        return false;
    }

    return writeEncData(originalFile, illFileSize, newName, errorMsg);
}


bool FNEDCore::encryptFile_withArg(const QString& fileAbsPath, int arg, QString& errorMsg)
{

    QString newName;
    auto bHasNewFileNameFlag = genNewFileName(fileAbsPath, newName, arg, true, errorMsg);
    if ( !bHasNewFileNameFlag ) {
        qDebug() << "generate new file name failed :  " << errorMsg;
        return false;
    }


    //
    // New file name is not exists , create a new file
    //
    // get the real file size in bytes
    QFile originalFile( fileAbsPath );
    if ( !originalFile.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }
    qint64 illFileSize = originalFile.size();
    if ( illFileSize <= 1 ) {
        errorMsg = QString("File size is %1, Do Nothing. ").arg( illFileSize );
        return false;
    }

    return writeEncDataWithArgs(originalFile, illFileSize, newName, arg, errorMsg); 
}



bool FNEDCore::encryptFile_TestAll(const QString& fileAbsPath,  QString& errorMsg)
{
    QFileInfo fInfo(fileAbsPath);
    QFile originalFile( fileAbsPath );
    if ( !originalFile.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }
    // get the real file size in bytes
    qint64 illFileSize = originalFile.size();

    auto bNoSuffixFlag = fInfo.suffix().isEmpty();
    QString newName;

    EncArgWrap argObj;
    auto ranMachine = QRandomGenerator::global();
    for ( auto i = 4; i<=6; ++i ) { // [4~6]
        for ( auto j = 0; j<=1; ++j ) { // [0,1]   0:Left    1:Right
            for ( auto k = 1; k<=7; ++k ) { // bitshift 1~7
                for ( auto lit = 0; lit<=1; ++lit ) { // 0:BigEndian   1:LittleEndian
                    newName = QString("%1/%2_%3-flag%4-%5%6-%7").arg( fInfo.absolutePath() )
                                                    .arg( fInfo.baseName() )
                                                    .arg( sc_encTest_Suffix )
                                                    .arg( i )
                                                    .arg( FNEDCore::s_debugLR[j] )
                                                    .arg( k )
                                                    .arg( FNEDCore::s_debugBigLittle[lit] );

                    if ( !bNoSuffixFlag ) {
                        newName += QString(".%1").arg( fInfo.suffix() );
                    } 

                    originalFile.seek( 0 );


                    argObj.additionalExp = i;
                    argObj.specialCharIdx = (i==4 ? 3 : s_aryWithout3[ ranMachine->bounded( static_cast<int>( sizeof(s_aryWithout3) / sizeof(s_aryWithout3[0]) ) ) ] );
                    argObj.nShiftBits = k;
                    argObj.bShiftDir = j;
                    argObj.bBigOrLittle = lit;

                    if ( !writeEncDataWithArgs(originalFile, illFileSize, newName, genIDByArgs(argObj), errorMsg) ) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}



bool FNEDCore::decryptFile_TestAll_WithCmp(const QString& fileAbsPath,  QString& errorMsg)
{

    QFileInfo fInfo(fileAbsPath);
    QFile originalFile( fileAbsPath );
    if ( !originalFile.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }
    // get the real file size in bytes
    // qint64 illFileSize = originalFile.size();

    auto bNoSuffixFlag = fInfo.suffix().isEmpty();
    QString newName;
    QString outputDecName;

    for ( auto i = 4; i<=6; ++i ) { // [4~6]
        for ( auto j = 0; j<=1; ++j ) { // [0,1]   0:Left    1:Right
            for ( auto k = 1; k<=7; ++k ) { // bitshift 1~7
                for ( auto lit = 0; lit<=1; ++lit ) { // 0:BigEndian   1:LittleEndian
                    newName = QString("%1/%2_%3-flag%4-%5%6-%7").arg( fInfo.absolutePath() )
                                                    .arg( fInfo.baseName() )
                                                    .arg( sc_encTest_Suffix )
                                                    .arg( i )
                                                    .arg( FNEDCore::s_debugLR[j] )
                                                    .arg( k )
                                                    .arg( FNEDCore::s_debugBigLittle[lit] );

                    if ( !bNoSuffixFlag ) {
                        newName += QString(".%1").arg( fInfo.suffix() );
                    } 

                    originalFile.seek( 0 );
                    if ( !decryptFile(newName, outputDecName, errorMsg ) ) {
                        errorMsg += QString(" , decode file failed on [%1]").arg( newName );
                        return false;
                    }

                    if ( !FNEDCore::is2FileEqual(fileAbsPath, outputDecName, errorMsg) ) {
                        errorMsg += QString(" , for the file content [%1] != [%2] ").arg( fileAbsPath).arg( outputDecName );
                        return false;
                    }
                }
            }
        }
    }

    return true;
}





bool FNEDCore::writeEncData(QFile& originalFileObj, qint64 originalFileSz, const QString& newFileName, QString& errorMsg)
{
    QFile encFile(newFileName);
    if ( !encFile.open( QIODevice::WriteOnly) ) {
        errorMsg = QString("Can't create the newFile for encrypt").arg( newFileName );
        return false;
    }

    auto ranMachine = QRandomGenerator::global();

    int additionalExponentFlag     = ranMachine->bounded(4, 7);  // [4,6]
    int ranCharIdx = ranMachine->bounded(0, SPECIAL_ELEMENTS);

    int ranShiftBits      = ranMachine->bounded(sc_maxint) % 7 + 1; // [0,6]  + 1          -> [1,7]
    int ranShiftDirection = ranMachine->bounded(sc_maxint) % 2;     // [0,1]  0:<<            1:>>
    int ranLitterEndian   = ranMachine->bounded(sc_maxint) % 2;     // [0,1]  0:Big Endian    1:Little Endian

    qint64 additionalSz = (sc_bitsInShort << (additionalExponentFlag-4));
    qint64 newFileSize = originalFileSz + additionalSz;

    // core core core
    encFile.resize( newFileSize );
    encFile.flush(); // TODO   comment or not ?

    auto bRet = writeHead3Bytes( encFile, additionalExponentFlag, ranCharIdx)
           &&   writeNextDummyBytes( encFile, additionalExponentFlag )
           &&   writeCore10Bytes( encFile, originalFileSz, ranShiftBits,  (ranLitterEndian==1) )
           &&   writeOriginalEncData( encFile, originalFileObj, originalFileSz, additionalExponentFlag,  ranShiftBits, (ranShiftDirection==1), errorMsg )
           &&   writeTailDummyBytes( encFile, additionalExponentFlag )
           &&   writeTailKeyBytes( encFile, additionalExponentFlag, ranCharIdx, (ranLitterEndian==1), (ranShiftDirection==1) );

    encFile.flush();
    encFile.close();
    return bRet;
}

bool FNEDCore::writeEncDataWithArgs(QFile& originalFileObj, qint64 originalFileSz, const QString& newFileName, int buildArgs, QString& errorMsg)
{
    QFile encFile(newFileName);
    if ( !encFile.open( QIODevice::WriteOnly) ) {
        errorMsg = QString("Can't create the newFile for encrypt").arg( newFileName );
        return false;
    }


    // int additionalExponentFlag     = ranMachine->bounded(4, 7);  // [4,6]
    int additionalExponentFlag      = static_cast<int>( (buildArgs &  ((1<<3)-1) ) );
    // int ranCharIdx = ranMachine->bounded(0, SPECIAL_ELEMENTS);
    int ranCharIdx = 0;
    if ( additionalExponentFlag == 4 ) {
        ranCharIdx = 3;
    } else {
        ranCharIdx = static_cast<int>( ((buildArgs>>3) &  ((1<<3)-1) ) );
        if ( ranCharIdx == 3 ) {
            auto rMachine = QRandomGenerator::global();
            int rIdx   = rMachine->bounded( static_cast<int>( sizeof(s_aryWithout3) / sizeof(s_aryWithout3[0]) ) );
            ranCharIdx = s_aryWithout3[rIdx];
        }
    }

    // int ranShiftBits      = ranMachine->bounded(sc_maxint) % 7 + 1; // [0,6]  + 1          -> [1,7]
    int ranShiftBits      = static_cast<int>( ((buildArgs>>6) &  ((1<<3)-1) ) );
    // int ranShiftDirection = ranMachine->bounded(sc_maxint) % 2;     // [0,1]  0:<<            1:>>
    int ranShiftDirection = static_cast<int>( (buildArgs>>7) &  0x1  ); 
    // int ranLitterEndian   = ranMachine->bounded(sc_maxint) % 2;     // [0,1]  0:Big Endian    1:Little Endian
    int ranLitterEndian   = static_cast<int>( (buildArgs>>8) &  0x1  );     // [0,1]  0:Big Endian    1:Little Endian

    qint64 additionalSz = (sc_bitsInShort << (additionalExponentFlag-4));
    qint64 newFileSize = originalFileSz + additionalSz;

    // core core core
    encFile.resize( newFileSize );
    encFile.flush(); // TODO   comment or not ?

    auto bRet = writeHead3Bytes( encFile, additionalExponentFlag, ranCharIdx)
           &&   writeNextDummyBytes( encFile, additionalExponentFlag )
           &&   writeCore10Bytes( encFile, originalFileSz, ranShiftBits,  (ranLitterEndian==1) )
           &&   writeOriginalEncData( encFile, originalFileObj, originalFileSz, additionalExponentFlag,  ranShiftBits, (ranShiftDirection==1), errorMsg )
           &&   writeTailDummyBytes( encFile, additionalExponentFlag )
           &&   writeTailKeyBytes( encFile, additionalExponentFlag, ranCharIdx, (ranLitterEndian==1), (ranShiftDirection==1) );

    if ( bRet ) {
        encFile.flush();
    }

    return bRet;
}



bool FNEDCore::decryptFile(const QString& fileAbsPath, QString& newFileName, QString& errorMsg)
{
    int additionalExponentFlag = 0;
    char specialCharAry[4] = { 0 };
    char core10Ary[10] = { 0 };
    bool bIsBigEndian    = false;
    bool bRightShiftFlag = false;
    unsigned int nShiftBits = 0;
    qint64 originalSz = 0;

    QFile encFile2Read( fileAbsPath );
    if ( !encFile2Read.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg(fileAbsPath);
        return false;
    }

    qint64 illFileSize = encFile2Read.size();
    if ( illFileSize <= 16 ) {
        errorMsg = QString("file '%1' size is so strange, size = %2").arg(fileAbsPath).arg( illFileSize );
        return false;
    }

    return    readHead3Bytes(encFile2Read, additionalExponentFlag, specialCharAry, errorMsg)
           && readTailKeyBytes(encFile2Read, illFileSize, additionalExponentFlag, specialCharAry, bIsBigEndian, bRightShiftFlag,  errorMsg)
           && skipReadDummyBytes(encFile2Read, additionalExponentFlag, errorMsg)
           && readCore10Bytes(encFile2Read, additionalExponentFlag, core10Ary, bIsBigEndian, originalSz, illFileSize, nShiftBits, errorMsg)
           && genNewFileName(fileAbsPath, newFileName, -1, false, errorMsg)
           && readEncDataAndThenWrite(encFile2Read, newFileName, originalSz, additionalExponentFlag, bRightShiftFlag, nShiftBits, errorMsg);

}


bool FNEDCore::readHead3Bytes(QFile& file2read, int& additionalExponentFlag, char* specialCharAry, QString& errorMsg)
{
    char additionalExponentChar = 0;
    qint64 readedBytes = 0;
    readedBytes = file2read.read(&additionalExponentChar, 1);
    if ( readedBytes!=1 ) {
        errorMsg = QString("Can't read the 1st byte from file %1 ").arg( file2read.fileName() );
        return false;
    }

    int n1Cnt = FNEDCore::howMany1InChar( additionalExponentChar );
    if ( !(n1Cnt>=2 && n1Cnt<=4) ) {
        errorMsg = QString("Leader byte is invalid!  n1Cnt = %1,  char = 0x%2").arg( n1Cnt ).arg( QString(QByteArray::number(static_cast<int>(additionalExponentChar), 16).toUpper()) );
        return false;
    }

    additionalExponentFlag = (n1Cnt + 2); // [2,4]   ==> [4,6]
    //                            &specialCharAry[0]
    readedBytes = file2read.read( (specialCharAry + 0), 2);
    if ( readedBytes!=2 ) {
        errorMsg = QString("Can't read the 2nd~3rd byte from file %1 ").arg( file2read.fileName() );
        return false;
    }

    return true;
}



bool FNEDCore::skipReadDummyBytes(QFile& file2read, int additionalExponentFlag, QString& errorMsg)
{
    bool bSeekRet = false;
    int skipByteCnt[] = { 3,11,27 };
    qint64 seekIdx = skipByteCnt[additionalExponentFlag-4];
    bSeekRet = file2read.seek(seekIdx);
    if ( !bSeekRet ) {
        errorMsg = QString("Can't seek head position @ %1 on original file \"%2\"!").arg( seekIdx  ).arg( file2read.fileName() );
        return false;
    }

    return true;
}


bool FNEDCore::readCore10Bytes(QFile& file2read, int additionalExponentFlag, char* core10Ary,  bool bIsBigEndian, qint64& originalSz, qint64 read2fileOriginalSz, unsigned int& nShiftBitCnt, QString& errorMsg)
{
    // char core10Ary[10] = { 0 };
    //                             &core10Ary[0]
    qint64 readedBytes = file2read.read(core10Ary, 10);
    if ( readedBytes!=10 ) {
        errorMsg = QString("Can't read core 10 bytes from file %1 ").arg( file2read.fileName() );
        return false;
    }

    if ( bIsBigEndian ) {
        originalSz =   ( (static_cast<unsigned int>(core10Ary[0]) & 0xFFu) << 24)
                     | ( (static_cast<unsigned int>(core10Ary[4]) & 0xFFu) << 16)
                     | ( (static_cast<unsigned int>(core10Ary[7]) & 0xFFu) <<  8)
                     | ( (static_cast<unsigned int>(core10Ary[9]) & 0xFFu)      );
    } else {
        originalSz =   ( (static_cast<unsigned int>(core10Ary[9]) & 0xFFu) << 24)
                     | ( (static_cast<unsigned int>(core10Ary[7]) & 0xFFu) << 16)
                     | ( (static_cast<unsigned int>(core10Ary[4]) & 0xFFu) <<  8)
                     | ( (static_cast<unsigned int>(core10Ary[0]) & 0xFFu)      );
    }

    if ( originalSz == 0 ) {
        errorMsg = QString("Combine coredata:fileSze from \"%1\", But it's file size = 0. ").arg( file2read.fileName() );
        return false;
    }
    
    if ( read2fileOriginalSz != (originalSz + (sc_bitsInShort << (additionalExponentFlag-4))) ) {
        errorMsg = QString("saved file combine size != originalFile's size. %1 != %2").arg( originalSz ).arg( read2fileOriginalSz );
        return false;
    }

    int idxAry[6] = { 1,2,3,  5,6,   8 };
    nShiftBitCnt = 0;
    for ( int i = 0; i < static_cast<int>( sizeof(idxAry) / sizeof(idxAry[0]) ); ++i )
    {
        nShiftBitCnt += static_cast<unsigned int>( howMany1InChar( core10Ary[ idxAry[i] ] ) );
    }

    return true;
}


bool FNEDCore::readTailKeyBytes(QFile& file2read, qint64 illFileSize,int additionalExponentFlag,char* spChAry, bool& bIsBigEndian, bool& bRightShiftFlag,  QString& errorMsg)
{
    qint64 seekIdx = 0;
    char tail4Ary[4] = { 0 };
    if ( additionalExponentFlag == 4 ) {
        seekIdx = illFileSize - 3;
    } else {
        seekIdx = illFileSize - 4;
    }
    bool bSeekRet = file2read.seek(seekIdx);
    if ( !bSeekRet ) {
        errorMsg = QString("Can't seek tail position @ %1 on original file \"%2\"!").arg( seekIdx  ).arg( file2read.fileName() );
        return false;
    }

    int tailReadCnt = (additionalExponentFlag==4 ? 3 : 4);
    qint64 readedBytes = file2read.read(&tail4Ary[0], tailReadCnt );
    if ( readedBytes!=tailReadCnt ) {
        errorMsg = QString("Can't read tail 3 or 4 bytes from file %1 ").arg( file2read.fileName() );
        return false;
    }

    // core core core
    bIsBigEndian = (FNEDCore::getNBit( tail4Ary[0], 4) == 0);

    QByteArray ba;
    ba.append(spChAry[0]);
    ba.append(spChAry[1]);
    if ( additionalExponentFlag==4 ) {
        spChAry[2] = tail4Ary[1];
        ba.append(spChAry[2]);
    } else {
        spChAry[2] = tail4Ary[1];
        spChAry[3] = tail4Ary[2];
        ba.append(spChAry[2]);
        ba.append(spChAry[3]);
    }
    char lastCh = tail4Ary[additionalExponentFlag==4 ? 2 : 3];
    // core core core
    bRightShiftFlag = (FNEDCore::getNBit( lastCh, 2) == 1);

    bool bIsSpecialCh = false;
    QString spCharacter(ba);
    for( int i = 0; i < SPECIAL_ELEMENTS; ++i ) {
        if ( spCharacter == FNEDCore::s_special_str_ary[i] ) {
            bIsSpecialCh = true;
            break;
        }
    }

    if ( !bIsSpecialCh ) {
        errorMsg = QString("'%1' is not inside the special character range ").arg( spCharacter );
        return false;
    }

    return true;
}


bool FNEDCore::readEncDataAndThenWrite(QFile& file2read, const QString& newFileName, qint64 originalFileSz, int additionalExponentFlag, bool bRightShiftFlag, unsigned int nShiftBits, QString& errorMsg)
{
    QFile outputFile(newFileName);
    if ( !outputFile.open( QIODevice::WriteOnly) ) {
        errorMsg = QString("Can't create the newFile for restore to originalFile. ").arg( newFileName );
        return false;
    }
    outputFile.resize( originalFileSz );

    qint64 startReadingOriginalPos[3] = { 13, 21, 37 };

    qint64 baseIdx = startReadingOriginalPos[additionalExponentFlag-4];
    bool bSeekRet = file2read.seek( baseIdx );
    if ( !bSeekRet ) {
        errorMsg = QString("Can't seek tail position @ %1 on original file \"%2\"!").arg( baseIdx  ).arg( file2read.fileName() );
        return false;
    }

    bool bHasError = false;
    if ( originalFileSz < (sc_ll_threshold*2) ) {
        bHasError = !readEncDataAndThenWrite4NormalFile(file2read, outputFile, originalFileSz, bRightShiftFlag, nShiftBits, errorMsg);
    } else {
        bHasError = !readEncDataAndThenWrite4LargeFile(file2read, outputFile, baseIdx, originalFileSz, bRightShiftFlag, nShiftBits, errorMsg);
    }
    
    if ( !bHasError ) {
        outputFile.flush();
    }

    return !bHasError;
}

bool FNEDCore::readEncDataAndThenWrite4NormalFile(QFile& file2read, QFile& wrtFile, qint64 originalFileSz, bool bRightShiftFlag,unsigned int nShiftBitCnt,  QString& errorMsg )
{
    char fileBuf[sc_ll_threshold*2] = { 0 };
    qint64 readedBytes = file2read.read( &fileBuf[0], originalFileSz);
    if ( readedBytes != originalFileSz ) {
        errorMsg = QString("[LargeFile] Can't read enough bytes on original file [%1] , already readed = %2 !").arg( file2read.fileName()  ).arg( readedBytes );
        return false;
    } 

    qint64 half = originalFileSz / 2;
    for( qint64 i = 0; i < half; ++i ) {
        //                                                   left <--> right  reverse the shift-bit direction
        bitShift2(fileBuf[i], fileBuf[(originalFileSz-1)-i], !bRightShiftFlag, nShiftBitCnt);
    }

    if ( originalFileSz % 2 == 1 ) {
        //                      left <--> right  reverse the shift-bit direction
        bitShift1Char_2( fileBuf[half], !bRightShiftFlag, nShiftBitCnt);
    }

    wrtFile.seek(0);
    qint64 wrtBytes = wrtFile.write(&fileBuf[0], originalFileSz );
    if ( wrtBytes != originalFileSz ) {
        errorMsg = QString("[LargeFile] Can't read enough bytes on original file [%1] , already readed = %2 !").arg( file2read.fileName()  ).arg( readedBytes );
        return false;
    }
    wrtFile.flush();

    return true;
}

bool FNEDCore::readEncDataAndThenWrite4LargeFile(QFile& file2read, QFile& wrtFile, qint64 baseIdx, qint64 originalFileSz, bool bRightShiftFlag, unsigned int nShiftBitCnt, QString& errorMsg )
{
    char leftPart[sc_ll_threshold]  = { 0 };
    char rightPart[sc_ll_threshold] = { 0 };
    const qint64 prBytes = (2*sc_ll_threshold);
    qint64 prCnt = originalFileSz / prBytes;
    qint64 restCnt  = originalFileSz % prBytes;

    qint64 seekIdx = 0;
    qint64 readedBytes = 0;
    qint64 wroteBytes = 0;
    bool bSeekRet = false;
    bool bHasForError = false;

    for( qint64 i = 0; i < prCnt; ++i )
    {
        // jump to nth-top
        seekIdx  = baseIdx + i*sc_ll_threshold ;
        bSeekRet = file2read.seek( seekIdx );
        if ( !bSeekRet ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't seek begin position %1 on original file !").arg( seekIdx  );
            break;
        }

        readedBytes = file2read.read(&leftPart[0], sc_ll_threshold);
        if ( readedBytes!=sc_ll_threshold ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't read begin enough bytes %1 on original file !").arg( readedBytes  );
            break;
        }


        // jump to nth (tail - sc_ll_threshold)
        seekIdx  = (baseIdx + originalFileSz) - (i+1) *sc_ll_threshold;
        bSeekRet = file2read.seek( seekIdx );
        if ( !bSeekRet ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't seek end position %1 on original file !").arg( seekIdx  );
            break;
        }

        readedBytes = file2read.read(&rightPart[0], sc_ll_threshold);
        if ( readedBytes!=sc_ll_threshold ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't read enough end bytes %1 on original file !").arg( readedBytes  );
            break;
        }

        //
        // shift bits
        //
        for( qint64 i = 0; i < sc_ll_threshold; ++i ) {
        //                                                           left <--> right  reverse the shift-bit direction
            bitShift2(leftPart[i], rightPart[(sc_ll_threshold-1)-i], !bRightShiftFlag , nShiftBitCnt);
        }

        //
        // Write original bytes
        //
        seekIdx  = 0 + (i*sc_ll_threshold);
        bSeekRet = wrtFile.seek( seekIdx );
        if ( !bSeekRet ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't seek begin position %1 on written file !").arg( seekIdx  );
            break;
        }

        wroteBytes =  wrtFile.write(&leftPart[0], sc_ll_threshold);
        if ( wroteBytes != sc_ll_threshold ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't wrote enough %1 bytes on written file !").arg( wroteBytes  );
            break;
        }


        seekIdx  = originalFileSz - (i+1) * sc_ll_threshold;
        bSeekRet = wrtFile.seek( seekIdx );
        if ( !bSeekRet ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't seek end position %1 on written file !").arg( seekIdx  );
            break;
        }

        wroteBytes =  wrtFile.write(&rightPart[0], sc_ll_threshold);
        if ( wroteBytes != sc_ll_threshold ) {
            bHasForError = true;
            errorMsg = QString("[LargeFile] Can't wrote enough %1 bytes on written file !").arg( wroteBytes  );
            break;
        }
    }

    if ( bHasForError ) {
        return false;
    }

    if ( restCnt > 0 ) {
        seekIdx  = baseIdx + (prCnt * sc_ll_threshold);
        bSeekRet = file2read.seek( seekIdx );
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek end position %1 on written file !").arg( seekIdx  );
            return false;
        }

        readedBytes = file2read.read(&leftPart[0], restCnt);
        if ( readedBytes!=restCnt  ) {
            errorMsg = QString("[LargeFile] Can't read rest enough bytes %1 on original file !").arg( readedBytes  );
            return false;
        }

        qint64 half = restCnt / 2;
        for( qint64 i = 0; i < half; ++i ) {
            //                                             left <--> right  reverse the shift-bit direction
            bitShift2(leftPart[i], leftPart[(restCnt-1)-i], !bRightShiftFlag, nShiftBitCnt);
        }

        if ( restCnt % 2 == 1 ) {
            //                             left <--> right  reverse the shift-bit direction
            bitShift1Char_2(leftPart[half], !bRightShiftFlag, nShiftBitCnt);
        }

        seekIdx  = ( 0 + (prCnt*sc_ll_threshold) );
        bSeekRet = wrtFile.seek( seekIdx );
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek rest position %1 on written file !").arg( seekIdx  );
            return false;
        }

        wroteBytes =  wrtFile.write(&leftPart[0], restCnt);
        if ( wroteBytes != restCnt ) {
            errorMsg = QString("[LargeFile] Can't wrote enough rest %1 bytes on written file !").arg( wroteBytes  );
            return false;
        }

        wrtFile.flush();
    }

    return true;
}




// static                                         right is true                     // [1,7]
void FNEDCore::bitShift1(char& retLeft, char& retRight, bool bShiftRightFlag, unsigned int nShiftBits)
{
    unsigned int iLeft  = static_cast<unsigned int>(retLeft  & 0xFF);
    unsigned int iRight = static_cast<unsigned int>(retRight & 0xFF);

    unsigned int cutBits    = 0;
    unsigned int placedBits = 0;
    if ( bShiftRightFlag ) {
        cutBits    = iRight & ( (1 << nShiftBits) -1 );
        placedBits = iLeft  & ( (1 << nShiftBits) -1 );
         
        retLeft  = static_cast<char>( (iLeft  >> nShiftBits) | (cutBits    << (sc_bitsInByte - nShiftBits)) );
        retRight = static_cast<char>( (iRight >> nShiftBits) | (placedBits << (sc_bitsInByte - nShiftBits)) );
    } else {
        cutBits    = iRight >> ( sc_bitsInByte - nShiftBits );
        placedBits = iLeft  >> ( sc_bitsInByte - nShiftBits );

        retLeft  = static_cast<char>( ( ((iLeft  << nShiftBits) | cutBits)    & 0xFF ) );
        retRight = static_cast<char>( ( ((iRight << nShiftBits) | placedBits) & 0xFF ) );
    }

}


void FNEDCore::bitShift2(char& retLeft, char& retRight, bool bShiftRightFlag, unsigned int nShiftBits)
{
    unsigned int iLeft  = static_cast<unsigned int>(retLeft  & 0xFF);
    unsigned int iRight = static_cast<unsigned int>(retRight & 0xFF);
    unsigned int combineShort = (iLeft << sc_bitsInByte) | iRight;

    unsigned int finalRet = 0u;
    if ( bShiftRightFlag ) {
        finalRet = (   (combineShort  >> nShiftBits)
                     | ( (combineShort   & ((1 << nShiftBits)-1) ) << (sc_bitsInShort - nShiftBits) )
                   )    & 0xFFFF;

    } else {
        finalRet =  (   (   combineShort << nShiftBits) 
                      | ( combineShort >> (sc_bitsInShort - nShiftBits) )
                );   // & 0xFFFF;
    }

    retLeft  = static_cast<char>( (finalRet >> sc_bitsInByte) & 0xFF );
    retRight = static_cast<char>( finalRet & 0xFF );

}


// static 
void FNEDCore::bitShift1Char_1(char& ch,  bool bShiftRightFlag, unsigned int nShiftBits )
{
    unsigned int iCh  = static_cast<unsigned int>(ch & 0xFF);
    unsigned int iRet = 0;
    if ( bShiftRightFlag ) {
        iRet = (iCh >> nShiftBits) | ( ( iCh & ((1 << nShiftBits)-1) ) << (sc_bitsInByte-nShiftBits) );
    } else {
        iRet  = ( iCh >> (sc_bitsInByte-nShiftBits) ) | ( (iCh << nShiftBits) );
    }

    ch = static_cast<char>( iRet );
    // ch = static_cast<char>( iRet & 0xFF );
}



// static 
void FNEDCore::bitShift1Char_2(char& ch,  bool bShiftRightFlag, unsigned int nShiftBits )
{
    unsigned int iCh  = static_cast<unsigned int>(ch & 0xFF);
    if ( bShiftRightFlag ) {
        iCh = (iCh >> nShiftBits) | ( (iCh & (( 1 << nShiftBits)-1)) << (8-nShiftBits) );
    } else {
        iCh = (iCh << nShiftBits) | (iCh >> (8-nShiftBits));
    }

    // ch = static_cast<char>( iCh & 0xFF );
    ch = static_cast<char>( iCh );
}


//                                                 [4,6]
bool FNEDCore::writeHead3Bytes(QFile& encFile, int additionalExponentFlag, int& ranCharIdx)
{
    auto rMachine = QRandomGenerator::global();

    int mapIdx1 = (additionalExponentFlag-2) - 1;  // [4,6] ==> [2,4] ==> index = [1,3]
    int pickedSz = FNEDCore::s_1Cnt_ary[ mapIdx1 ].size();
    char chByte  = static_cast<char>( FNEDCore::s_1Cnt_ary[ mapIdx1 ][ rMachine->bounded( pickedSz ) ] & 0xFF );
    //       -2
    // [4~6] => [2~4]    write a byte with  2/3/4 bits of 1
    encFile.write(&chByte, 1);

    if ( additionalExponentFlag == 4 ) {
        // [4,6]  -> [2,4]
        ranCharIdx = 3;
    } else {
        if ( ranCharIdx == 3 ) {
           int rIdx   = rMachine->bounded( static_cast<int>( sizeof(s_aryWithout3) / sizeof(s_aryWithout3[0]) ) );
           ranCharIdx = s_aryWithout3[rIdx];
        }
    }

    chByte = FNEDCore::s_special_ary[ranCharIdx].chars[0];
    encFile.write(&chByte, 1);
    chByte = FNEDCore::s_special_ary[ranCharIdx].chars[1];
    encFile.write(&chByte, 1);

    return true;
}


bool FNEDCore::writeNextDummyBytes(QFile& encFile, int additionalExponentFlag)
{
    if ( additionalExponentFlag != 4 ) {
        auto rMachine = QRandomGenerator::global();
        int nBytes = (additionalExponentFlag == 5 ? 8 : 24);

        char chByte = 0;
        for( auto i = 0; i < nBytes; ++i ) {
            chByte = FNEDCore::s_randomDummyCharAry[ rMachine->bounded( DUMMY_ARY_SIZE )  ];
            encFile.write(&chByte, 1);
        }
    }

    return true;
}


bool FNEDCore::writeCore10Bytes(QFile& encFile, qint64 originalFileSz, int nShiftBits, bool bIsLittleEndian)
{
    auto rMachine = QRandomGenerator::global();
    QSet<int> szIdxAry{  0,     4,     7,    9 };
    QSet<int> bitIdxSet{ 1,2,3,    5,6,   8    }; // [1,2,3   5,6,   8]  are the sub-indices of the 1-bit

    char szBytes[] = {
        static_cast<char>( (originalFileSz >> 24) & 0xFF),
        static_cast<char>( (originalFileSz >> 16) & 0xFF),
        static_cast<char>( (originalFileSz >>  8) & 0xFF),
        static_cast<char>(  originalFileSz        & 0xFF)
    };

    int baseIdx = ( bIsLittleEndian ? ( sizeof(szBytes) / sizeof(szBytes[0]) - 1 ) : 0 );

    // 6 slots
    // QMap<int,int> szIdxAry2; // [1,2,3   5,6,   8]  are the sub-indices of the 1-bit
    // szIdxAry2.insert(1,0);
    // szIdxAry2.insert(2,0);
    // szIdxAry2.insert(3,0);    
    // szIdxAry2.insert(5,0);
    // szIdxAry2.insert(6,0); 
    // szIdxAry2.insert(8,0); 

    // 6 slots
    using namespace std;
    QMap<int,int> szIdxAry2 {
        make_pair(1,0),
        make_pair(2,0),
        make_pair(3,0),
        make_pair(5,0),
        make_pair(6,0),
        make_pair(8,0),
    };

    if ( nShiftBits == 6 ||  nShiftBits == 7 ) {
        for( auto it = szIdxAry2.begin(); it!=szIdxAry2.end(); ++it ) {
            it.value() = 1;
        }

        if ( nShiftBits == 7 ) {
            int iPos = rMachine->bounded( bitIdxSet.size() );
            (szIdxAry2.begin() + iPos).value() = 2;
        }
    } else {
        // 1~5
        int pickedCnt = 0;
        while ( pickedCnt < nShiftBits ) {
            int iPos = rMachine->bounded( bitIdxSet.size() );
            auto pIt = bitIdxSet.begin() + iPos;

            int val = *pIt;
            auto findIt = szIdxAry2.find(val);
            if ( findIt != szIdxAry2.end() ) {
                findIt.value() = 1;
            }

            bitIdxSet.erase( pIt );
            ++pickedCnt;
        }
    }


    int oneSz = FNEDCore::s_1Cnt_ary[0].size();
    int twoSz = FNEDCore::s_1Cnt_ary[1].size();
    char chByte = 0;
    // 6 bytes with   nShiftBits 1(s)
    for ( auto i = 0; i < 10; ++i ) {
        if ( szIdxAry.find(i) != szIdxAry.end() ) {
            chByte = szBytes[baseIdx];
            encFile.write(&chByte, 1);
            if ( bIsLittleEndian ) {
                --baseIdx;
            } else {
                ++baseIdx;
            }
        } else {
            auto findIt = szIdxAry2.find(i);
            if ( findIt != szIdxAry2.end() ) {
                int oneBitCnt = findIt.value();
                if ( oneBitCnt == 0 ) {
                    chByte = 0;
                } else if ( oneBitCnt == 1 ) {
                    chByte = static_cast<char>( FNEDCore::s_1Cnt_ary[0][ rMachine->bounded( oneSz ) ] & 0xFF );
                } else if ( oneBitCnt == 2 ) {
                    chByte = static_cast<char>( FNEDCore::s_1Cnt_ary[1][ rMachine->bounded( twoSz ) ] & 0xFF );
                }
                encFile.write(&chByte, 1);
            }
        }
    }

    return true;
}

bool FNEDCore::writeOriginalEncData_LargeFile(QFile& encFile, QFile& originalFile, qint64 originalFileSz, int nShiftBits, bool bRightShiftFlag, qint64 base,QString& errorMsg)
{
    const qint64 pair_Bytes = (2 * sc_ll_threshold);
    qint64 prCnt   = originalFileSz / pair_Bytes;
    qint64 restCnt = originalFileSz % pair_Bytes;

    char leftPart[sc_ll_threshold]  = { 0 };
    char rightPart[sc_ll_threshold] = { 0 };

    qint64 seekIdx = 0;
    qint64 readedBytes = 0;
    qint64 writtenBytes = 0;
    bool bSeekRet = false;
    auto bHasError = false;

    for ( qint64 i = 0; i < prCnt; ++i ) {
        seekIdx = 0 + i*sc_ll_threshold; // for left part
        bSeekRet = originalFile.seek(seekIdx);
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek head position %1 on original file !").arg( seekIdx  );
            bHasError = true;
            break;
        }
        readedBytes = originalFile.read(&leftPart[0], sc_ll_threshold);
        if ( readedBytes != sc_ll_threshold ) {
            errorMsg = QString("[LargeFile] Can't read enough bytes @ position %1 on original file , left already readed = %2 !").arg( seekIdx  ).arg( readedBytes );
            bHasError = true;
            break;
        }

        seekIdx = originalFileSz - (i+1)*sc_ll_threshold; // for right part
        bSeekRet = originalFile.seek(seekIdx);
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek tail position %1 on original file !").arg( seekIdx  );
            bHasError = true;
            break;
        }
        readedBytes = originalFile.read(&rightPart[0], sc_ll_threshold);
        if ( readedBytes != sc_ll_threshold ) {
            errorMsg = QString("[LargeFile] Can't read enough bytes @ position %1 on original file , right already readed = %2 !").arg( seekIdx  ).arg( readedBytes );
            bHasError = true;
            break;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Core Core Core    Shift bytes
        //
        //     Shift-Bit 
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        for ( qint64 j = 0; j < sc_ll_threshold; ++j ) {
            FNEDCore::bitShift1(leftPart[j], rightPart[(sc_ll_threshold-1)-j], bRightShiftFlag, static_cast<unsigned int>( nShiftBits ) );
        }


        // write <Left> Part
        seekIdx = base + (0 + i*sc_ll_threshold); // move cursor to the encrypt left part
        bSeekRet = encFile.seek( seekIdx );
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek head position %1 on encrypt file !").arg( seekIdx  );
            bHasError = true;
            break;
        }
        writtenBytes = encFile.write(&leftPart[0], sc_ll_threshold);
        if ( writtenBytes != sc_ll_threshold ) {
            errorMsg = QString("[LargeFile] Can't write enough bytes @ position %1 on encrypt file , <Left> already wrote = %2 !").arg( seekIdx  ).arg( writtenBytes );
            bHasError = true;
            break;
        }

        // write <Right> Part
        seekIdx = (base + originalFileSz) - (i+1)*sc_ll_threshold; // for right part
        bSeekRet = encFile.seek(seekIdx);
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek tail position %1 on encrypt file !").arg( seekIdx  );
            bHasError = true;
            break;
        }

        writtenBytes = encFile.write(&rightPart[0], sc_ll_threshold);
        if ( writtenBytes != sc_ll_threshold ) {
            errorMsg = QString("[LargeFile] Can't write enough bytes @ position %1 on encrypt file , <Right> already wrote = %2 !").arg( seekIdx  ).arg( writtenBytes );
            bHasError = true;
            break;
        }
    }

    // Rest Part
    if ( !bHasError && restCnt > 0 ) {
        seekIdx = 0 + prCnt*sc_ll_threshold; // for rest part
        bSeekRet = originalFile.seek(seekIdx);
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek head position %1 on original file ( Rest Part ) !").arg( seekIdx  );
            return false;
        }

        readedBytes = originalFile.read(&leftPart[0], restCnt);
        if ( readedBytes != restCnt ) {
            errorMsg = QString("[LargeFile] Can't read enough bytes @ position %1 on original file , rest Part already readed = %2 !").arg( seekIdx  ).arg( readedBytes );
            return false;
        }

        // Core Core Core , do shift-bits
        qint64 half = restCnt / 2;
        for ( qint64 i = 0; i < half; ++i ) {
            bitShift2(leftPart[i], leftPart[(restCnt-1)-i], bRightShiftFlag, static_cast<unsigned int>(nShiftBits) );
        }

        // Process the Odd Byte 
        if ( restCnt % 2 == 1 ) {
            // shift the only byte
            bitShift1Char_2( leftPart[half], bRightShiftFlag, static_cast<unsigned int>(nShiftBits) );
        }

        seekIdx = base + (prCnt*sc_ll_threshold); // move cursor to the encrypt left part
        bSeekRet = encFile.seek( seekIdx );
        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek encFile position %1 on encrypt file for rest part !").arg( seekIdx  );
            return false;
        }

        writtenBytes = encFile.write(&leftPart[0], restCnt);
        if ( writtenBytes != restCnt ) {
            errorMsg = QString("[LargeFile] Can't write enough bytes @ position %1 on encrypt file , restPart already wrote = %2 !").arg( seekIdx  ).arg( writtenBytes );
            return false;
        }
    }

    if ( !bHasError ) {
        seekIdx = base + originalFileSz;
        bSeekRet = encFile.seek( seekIdx );

        if ( !bSeekRet ) {
            errorMsg = QString("[LargeFile] Can't seek encFile (for content) position %1 on encrypt file for rest part !").arg( seekIdx  );
            bHasError = true;
        }
    }

    return !bHasError;
}

bool FNEDCore::writeOriginalEncData_NormalFile(QFile& encFile, QFile& originalFile, qint64 originalFileSz,  int nShiftBits,bool bRightShiftFlag, qint64 base, QString& errorMsg)
{
    // to ensure the index will not out of range 
    char allBytes[sc_ll_threshold*2]  = { 0 };
    if ( !originalFile.seek(0) ) {
        errorMsg = QStringLiteral("Process Normal File , Can't seek to 0.");
        return false;
    }

    qint64 readedBytes = originalFile.read(&allBytes[0], sc_ll_threshold);
    if ( readedBytes != originalFileSz ) {
        errorMsg = QStringLiteral("Process Normal File , read_bytes != originalFileSz . ");
        return false;
    }

    // Core Core Core , do shift-bits
    qint64 half = readedBytes / 2;
    for ( qint64 i = 0; i < half; ++i ) {
        bitShift2( allBytes[i], allBytes[(readedBytes-1)-i], bRightShiftFlag, static_cast<unsigned int>(nShiftBits) );
    }

    // Process the Odd Byte 
    if ( readedBytes % 2 == 1 ) {
        bitShift1Char_2( allBytes[half], bRightShiftFlag, static_cast<unsigned int>(nShiftBits) );
    }

    qint64 seekIdx = base; // move cursor 
    bool bSeekRet = encFile.seek( seekIdx );
    if ( !bSeekRet ) {
        errorMsg = QString("[write NormalFile] Can't seek head position %1 on encrypt file !").arg( seekIdx  );
        return false;
    }

    qint64 writtenBytes = encFile.write(&allBytes[0], originalFileSz);
    if ( writtenBytes != originalFileSz ) {
        errorMsg = QString("[write NormalFile] Can't write enough bytes @ position %1 on encrypt file , already wrote = %2 !").arg( seekIdx ).arg( writtenBytes );
        return false;
    }

    return true;
}


bool FNEDCore::writeOriginalEncData(QFile& encFile, QFile& originalFile, qint64 originalFileSz, int additionalExponentFlag, int nShiftBits,bool bRightShiftFlag, QString& errorMsg)
{
    QMap<int,qint64> mp;
    mp.insert(4,13);
    mp.insert(5,21);
    mp.insert(6,37);

    auto it = mp.find(additionalExponentFlag);
    if ( it == mp.end() ) {
        errorMsg = QString("Can't find move steps by %1").arg( additionalExponentFlag );
        return false;
    }


    auto bHasError = false;
    qint64 base  = it.value();

    if ( originalFileSz <= sc_ll_threshold ) {
        bHasError = !writeOriginalEncData_NormalFile(encFile, originalFile, originalFileSz, nShiftBits, bRightShiftFlag, base, errorMsg );
    } else {
        // originalFileSz > sc_ll_threshold
        //                         2 * means   xxxx      ????    xxxx   ( the most left part && the most right part , each part contains sc_ll_threshold bytes )
        bHasError = !writeOriginalEncData_LargeFile(encFile, originalFile, originalFileSz, nShiftBits, bRightShiftFlag, base,  errorMsg );
    }

    if ( !bHasError ) {
        encFile.flush();
    }

    return !bHasError;
}


bool FNEDCore::writeTailDummyBytes(QFile& encFile, int additionalExponentFlag)
{
    if ( additionalExponentFlag != 4 ) {
        auto rMachine = QRandomGenerator::global();
        int nBytes = (additionalExponentFlag == 5 ? 7 : 23);

        char chByte = 0;
        for( auto i = 0; i < nBytes; ++i ) {
            chByte = FNEDCore::s_randomDummyCharAry[ rMachine->bounded( DUMMY_ARY_SIZE )  ];
            encFile.write(&chByte, 1);
        }
    }

    return true;
}


bool FNEDCore::writeTailKeyBytes(QFile& encFile, int additionalExponentFlag, int ranCharIdx, bool bIsLittleEndian, bool bRightShiftFlag)
{
    char chByte = 0;
    auto rMachine = QRandomGenerator::global();

    int bitIdx = 0;
    // 1. write bigEndian or LittleEndian  flag
    if ( bIsLittleEndian ) {
        bitIdx = 4;
        chByte = static_cast<char>( FNEDCore::s_nth1Cnt_ary[bitIdx][ rMachine->bounded( FNEDCore::s_nth1Cnt_ary[bitIdx].size() ) ] & 0xFF );   
    } else {
        bitIdx = 1;
        chByte = static_cast<char>( FNEDCore::s_nth0Cnt_ary[bitIdx][ rMachine->bounded( FNEDCore::s_nth0Cnt_ary[bitIdx].size() ) ] & 0xFF );   
    }
    encFile.write(&chByte, 1);

    // 2. write special characters bytes
    if ( additionalExponentFlag == 4 ) {
        chByte = FNEDCore::s_special_ary[ranCharIdx].chars[2];
        encFile.write(&chByte, 1);
    } else {
        chByte = FNEDCore::s_special_ary[ranCharIdx].chars[2];
        encFile.write(&chByte, 1);

        chByte = FNEDCore::s_special_ary[ranCharIdx].chars[3];
        encFile.write(&chByte, 1);
    }


    // 3. write <<   or   >>               flag
    if ( bRightShiftFlag  ) {
        bitIdx = 2;
        chByte = static_cast<char>( FNEDCore::s_nth1Cnt_ary[bitIdx][ rMachine->bounded( FNEDCore::s_nth1Cnt_ary[bitIdx].size() ) ] & 0xFF );   
    } else {
        bitIdx = 0;
        chByte = static_cast<char>( FNEDCore::s_nth0Cnt_ary[bitIdx][ rMachine->bounded( FNEDCore::s_nth0Cnt_ary[bitIdx].size() ) ] & 0xFF );   
    }
    encFile.write(&chByte, 1);

    return true;
}


// static
int FNEDCore::howMany1InChar(char ch)
{
    int cnt = 0;
    for( int i = 0; i < 8; ++i ) {
        if ( (( ch >> i ) & 0x1) == 1 ) {
            ++cnt;
        }
    }
    return cnt;
}

// static 
int FNEDCore::getNBit(char ch, int idx)
{
    return ( ( (ch >> idx) & 0x1 ) == 0x1)
        ? 1 : 0;
}

// static 
bool FNEDCore::is2FileEqual(const QString& fname1, const QString& fname2, QString& errorMsg)
{
    qint64 fSz1 = 0;
    qint64 fSz2 = 0;
    QFile file1(fname1);
    if ( !file1.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg( fname1 );
        return false;
    }
    fSz1 = file1.size();

    QFile file2(fname2);
    if ( !file2.open(QIODevice::ReadOnly) ) {
        errorMsg = QString("Open file '%1' for reading failed! ").arg( fname2 );
        return false;
    }
    fSz2 = file2.size();

    if ( fSz1 != fSz2 ) {
        errorMsg = QString("file1.size() != file2.size().   %1 != %2 ").arg( fSz1 ).arg( fSz2 );
        return false;
    }

    bool bIsSame = true;

    char buf1[sc_ll_cmpBlockSize] = { 0 };
    char buf2[sc_ll_cmpBlockSize] = { 0 };
    qint64 blockCnt = fSz1 / sc_ll_cmpBlockSize;
    qint64 restPart = fSz1 % sc_ll_cmpBlockSize;

    for( qint64 i = 0; i < blockCnt; ++i ) {
        file1.read(buf1, sc_ll_cmpBlockSize);
        file2.read(buf2, sc_ll_cmpBlockSize);

        for( qint64 j = 0; j < sc_ll_cmpBlockSize; ++j ) {
            if ( buf1[j] != buf2[j] ) {
                bIsSame = false;
                errorMsg = QString("2 char are not same. @position %1 , '%2' != %3").arg( (i*sc_ll_cmpBlockSize) + j ).arg( QString( QByteArray::number(static_cast<int>(buf1[j] & 0xFF), 16).toUpper() ) ).arg(  QString( QByteArray::number(static_cast<int>(buf2[j] & 0xFF), 16).toUpper() )  );
                break;
            }
        }

        if ( !bIsSame ) {
            break;
        }
    }

    if ( bIsSame && restPart > 0 ) {
        file1.read(buf1, restPart);
        file2.read(buf2, restPart);

        for( qint64 j = 0; j < restPart; ++j ) {
            if ( buf1[j] != buf2[j] ) {
                bIsSame = false;
                errorMsg = QString("2 char are not same. @position %1 , '%2' != %3").arg( (blockCnt*sc_ll_cmpBlockSize) + j ).arg( QString( QByteArray::number(static_cast<int>(buf1[j] & 0xFF), 16).toUpper() ) ).arg(  QString( QByteArray::number(static_cast<int>(buf2[j] & 0xFF), 16).toUpper() )  );
                break;
            }
        }
    }
    file1.close();
    file2.close();

    return bIsSame;
}



    
// static 
int  FNEDCore::genIDByArgs(const EncArgWrap& obj)
{
    return TO_ENC_ARGS(obj);
}


// static 
QString FNEDCore::changeBaseName(const QString& baseNameOnly, const QString& suffix, bool bIsEnc)
{
    if ( !sc_bUseNameChangeFunc ) {
        return baseNameOnly;
    }

    int matchFlag = 0;
    FNEDCore::changeSuffixName(suffix,bIsEnc, &matchFlag);

    static QString rule0[2] = { "DJI_", "dji_" };
    QString retStr = baseNameOnly;

    if ( bIsEnc ) {
        //////////////////////////////////////
        // Encrypt

        // process BaseName
        if ( (baseNameOnly!=rule0[0] && baseNameOnly!=rule0[1]) 
                && baseNameOnly.startsWith(rule0[0], Qt::CaseInsensitive) ) {
            retStr = baseNameOnly.mid( rule0[0].size() );
        } 
    } else {
        //////////////////////////////////////
        // Decrypt
        if ( matchFlag != 0 ) {
            if ( !baseNameOnly.startsWith(rule0[0], Qt::CaseInsensitive) ) {
                retStr.prepend( rule0[0] );
            } 
        }
    }

    return retStr;
}


// static 
QString FNEDCore::changeSuffixName(const QString& suffixOnly, bool bIsEnc, int* pbIsMatched)
{
    static QString rule1[2] = { "MP4", "MWV" };
    static QString rule2[2] = { "DNG", "RAW" };
    static QString rule3[2] = { "JPG", "webp" };
    static QString rule4[2] = { "JPEG", "webep" };


    // init
    if ( pbIsMatched!=nullptr ) {
        *pbIsMatched = 0;
    }

    if ( suffixOnly.isEmpty() || !sc_bUseNameChangeFunc ) {
        return suffixOnly;
    }

    // Covert it to Uppercase
    QString retSuffix = suffixOnly.toUpper();
    int matchedFlag = 0;
    if ( bIsEnc ) {
        if ( retSuffix == rule1[0] ) {
            matchedFlag = 1;
            retSuffix = rule1[1];
        } else if ( retSuffix == rule2[0] ) {
            matchedFlag = 2;
            retSuffix = rule2[1];
        } else if ( retSuffix == rule3[0] ) {
            matchedFlag = 3;
            retSuffix = rule3[1];
        } else if ( retSuffix == rule4[0] ) {
            matchedFlag = 4;
            retSuffix = rule4[1];
        }
    } else {
        if ( retSuffix == rule1[1] ) {
            matchedFlag = 1;
            retSuffix = rule1[0];
        } else if ( retSuffix == rule2[1] ) {
            matchedFlag = 2;
            retSuffix = rule2[0];
        } else if ( retSuffix == rule3[1] ) {
            matchedFlag = 3;
            retSuffix = rule3[0];
        } else if ( retSuffix == rule4[1] ) {
            matchedFlag = 4;
            retSuffix = rule4[0];
        }
    }

    if ( pbIsMatched!=nullptr && matchedFlag !=0 ) {
        *pbIsMatched = 1;
    }

    return retSuffix;
}


