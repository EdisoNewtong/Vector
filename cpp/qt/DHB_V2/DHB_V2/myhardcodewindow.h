#ifndef MY_HARD_CODE_WINDOW_H
#define MY_HARD_CODE_WINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QMap>

QT_BEGIN_NAMESPACE
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QRadioButton;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QGridLayout;
class QPushButton;
class QCheckBox;
QT_END_NAMESPACE

class myhardcodewindow : public QMainWindow
{
    Q_OBJECT

public:
    myhardcodewindow(QWidget* parent = nullptr);
    virtual ~myhardcodewindow();
    void initUI();

protected slots:
    void onUnsignedSignedClicked(int btnId, bool checkFlag);
    void onDataTypeClicked(int btnId, bool checkFlag);
    void onRepresentationClick(int btnId, bool checkFlag);
    void onCharRepresentationClick(int btnId, bool checkFlag);
    void onOctalSwitchBtnClicked(bool check);
    void updateDetailOutput();
    void doBinaryConvert();
    void onCopyBtnClicked(bool check);
protected:
    static const int sc_maxbitCnt = 64;
    static const int sc_byteVBarCnt = 9;
    // 22 = (64/3=21 ... 1) = 22
    static const int sc_octCtrlCnt = 22;
    // 64 bits 
    static const int sc_binaryCtrlCnt = 64;
    // 16 hex number => 64 bits
    static const int sc_hexCtrlCnt = 16;

    static const int sc_mapArySize = 4;

    void updateDataTypeInfo();
    void updateInputPromptInfo();
    void updateOctBinaryColor();

    void bindSignalAndSlot();
    inline unsigned int getUnsignedSigned();
    inline unsigned int getDataType();
    inline unsigned int getPresentStyle();
    inline unsigned int getCharPresentStyle();

    void convertFailedHints( const QString& s = QString() );
    void updateCharBits();
    void updateShortBits();
    void updateIntBits();
    void updateLongBits();
    void updateLongLongBits();
    void innerBitsUpdate(int nBits);
    void setAllBitsDash();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // To Fix a bug when converting signed short/int/long/long long while the binary highest bit is '1'
    bool isHightestBit_1(const QString& targetStr, size_t nBits, int baseOpt);
    QByteArray string2ByteArray_WithAlignmentByte(const QString& inStr, size_t nBits, int baseOpt);
    void octbaseProcessUgly(QByteArray& retAry, const QString& inStr, size_t nBits);
    bool octbaseProcessNice(QByteArray& retAry, const QString& inStr, size_t nBits, QString& errorMsg);
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    QString flagAuxInfo();
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI Control Part Begin
    QWidget*                m_pCenterWidget;
    QVBoxLayout*             m_pVBoxGrpAll; // for all

    QGroupBox*               m_pChkBoxGrp;
     QHBoxLayout*            m_pHBoxChkBoxGrp;
      // unsigned / signed
      QGroupBox*             m_pUnsignedSignedGrp;
      QButtonGroup*          m_pUnsignedSignedBtnsGrp;
      QVBoxLayout*           m_pVBoxUnsignedSigned;
        QRadioButton*          m_pUnsignedChkBox;
        QRadioButton*          m_pSignedChkBox;
      // Data Type : char / short / int / long / long
      QGroupBox*             m_pCharShortIntLongLLGrp;
      QButtonGroup*          m_pCharShortIntLongLLBtnsGrp;
      QVBoxLayout*           m_pVBoxDataType;
        QRadioButton*          m_pCharChkBox;
        QRadioButton*          m_pShortChkBox;
        QRadioButton*          m_pIntChkBox;
        QRadioButton*          m_pLongChkBox;
        QRadioButton*          m_pLongLongChkBox;
      // Representation : Binary / Oct / Hex / Decimal
      QGroupBox*             m_pRepresentationBinaryOctHexDecGrp;
      QButtonGroup*          m_pRepresentationBinaryOctHexDecBtnsGrp;
      QVBoxLayout*           m_pVBoxRepresentation;
        QRadioButton*           m_pBinaryChkBox;
        QRadioButton*           m_pOctChkBox;
        QRadioButton*           m_pHexChkBox;
        QRadioButton*           m_pDecimalChkBox;
      QGroupBox*             m_pRepresentationCharacterAsciiGrp;
      QButtonGroup*          m_pRepresentationCharacterAsciiBtnsGrp;
      QVBoxLayout*           m_pVBoxCharRepresentation;
        QRadioButton*          m_pCharacterChkBox;
        QRadioButton*          m_pAsciiChkBox;

      // Range Part
      QGroupBox*               m_pRangeBox;
      QHBoxLayout*             m_pHBoxRange;
      QLabel*                  m_pRangeLabel;
      QLineEdit*               m_pMinNumberText;
      QLabel*                  m_pToLabel;
      QLineEdit*               m_pMaxNumberText;

      // Input Parts
      QGroupBox*               m_pInputGrp;
      QHBoxLayout*             m_pHBoxInputParts;
      QLabel*                  m_pInputPrompt;
      QLineEdit*               m_pInputBox;
      QLabel*                  m_pEqualLabel;
      QLineEdit*               m_pDecialCharDetailBox;
      QCheckBox*               m_pFormatOutuptDetailBox;
      QPushButton*             m_pCopyToClipBoardBtn;

      // Display Binary / Hex / Oct
      QGridLayout*             m_pGridLayout;
      QGroupBox*               m_pDisplayGrp;
      // Oct
      QPushButton*             m_pOctHeaderButton;
      // QLabel*                  m_pOctVBarAry[];
      QLabel*                  m_pOctAry[sc_octCtrlCnt];
      // Binary
      QLabel*                  m_pBinHeaderLabel;
      QLabel*                  m_pBinIndexHeaderLabel;
      QLabel*                  m_pBinIndexAry[sc_binaryCtrlCnt];
      QLabel*                  m_pBinVBarAry[sc_byteVBarCnt];
      QLabel*                  m_pBinAry[sc_binaryCtrlCnt];
      // Hex
      QLabel*                  m_pHexHeaderLabel;
      QLabel*                  m_pHexVBarAry[sc_byteVBarCnt];
      QLabel*                  m_pHexAry[sc_hexCtrlCnt];

      QCheckBox*               m_pShowFullBitsChkBox;


      // Aux HelpUtil Button

      
    QStatusBar*             m_pStatusBar;
    // UI Control Part End
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  m_charBits;
    size_t                  m_shortBits;
    size_t                  m_intBits;
    size_t                  m_longBits;
    size_t                  m_longlongBits;

    const int               m_baseMap[sc_mapArySize];

    bool                    m_original2ColorMode;
    

    QByteArray              m_byteBin;
    QByteArray              m_byteOct;
    QByteArray              m_byteHex;
    QByteArray              m_byteDecimal;

    QString                 m_strColor1; 
    QString                 m_strColor2; 
    QString                 m_strColor3; 

    QString                 m_strDetailOutput;

    //                      | ? 1 + 2 | 3 + 1 |
    unsigned int            m_pickedInfo;
    const int               m_errorMsgShowTime; // in milli-seconds

    QMap<int,QString>                m_unprintedTable;

    QMap<QString, QString>           m_octNumberStringMap;
};



#endif
