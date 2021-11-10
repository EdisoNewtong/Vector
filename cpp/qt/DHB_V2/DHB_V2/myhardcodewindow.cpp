#include <QStatusBar>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QFont>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QRegExp>
#include <QApplication>
#include <QClipboard>
// #include <QByteArray>

#include <QDebug>

#include <limits>
#include "myhardcodewindow.h"

// Calc limited number
namespace limitedNumber
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // char
    static const unsigned char min_unsigned_char = std::numeric_limits<unsigned char>::min();
    static const unsigned char max_unsigned_char = std::numeric_limits<unsigned char>::max();
    static const  signed char  min_signed_char   = std::numeric_limits<signed char>::min();
    static const  signed char  max_signed_char   = std::numeric_limits<signed char>::max();
    // short
    static const unsigned short min_unsigned_short = std::numeric_limits<unsigned short>::min(); 
    static const unsigned short max_unsigned_short = std::numeric_limits<unsigned short>::max(); 
    static const signed short   min_signed_short   = std::numeric_limits<signed short>::min(); 
    static const signed short   max_signed_short   = std::numeric_limits<signed short>::max(); 
    // int
    static const unsigned int min_unsigned_int = std::numeric_limits<unsigned int>::min(); 
    static const unsigned int max_unsigned_int = std::numeric_limits<unsigned int>::max(); 
    static const signed int   min_signed_int   = std::numeric_limits<signed int>::min(); 
    static const signed int   max_signed_int   = std::numeric_limits<signed int>::max(); 
    // long
    static const unsigned long min_unsigned_long = std::numeric_limits<unsigned long>::min(); 
    static const unsigned long max_unsigned_long = std::numeric_limits<unsigned long>::max(); 
    static const signed long   min_signed_long   = std::numeric_limits<signed long>::min(); 
    static const signed long   max_signed_long   = std::numeric_limits<signed long>::max(); 
    // long long
    static const unsigned long long min_unsigned_longlong = std::numeric_limits<unsigned long long>::min(); 
    static const unsigned long long max_unsigned_longlong = std::numeric_limits<unsigned long long>::max(); 
    static const signed long  long min_signed_longlong    = std::numeric_limits<signed long long>::min(); 
    static const signed long  long max_signed_longlong    = std::numeric_limits<signed long long>::max(); 
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



// explicit
myhardcodewindow::myhardcodewindow(QWidget* parent /* = nullptr */  )
    : QMainWindow(parent)
    , m_pCenterWidget(nullptr)
    //
    , m_pVBoxGrpAll(nullptr)
    , m_pChkBoxGrp(nullptr)
    , m_pHBoxChkBoxGrp(nullptr)
    , m_pUnsignedSignedGrp(nullptr)
    , m_pUnsignedSignedBtnsGrp(nullptr)
    , m_pVBoxUnsignedSigned(nullptr)
    , m_pUnsignedChkBox(nullptr)
    , m_pSignedChkBox(nullptr)
    , m_pCharShortIntLongLLGrp(nullptr)
    , m_pCharShortIntLongLLBtnsGrp(nullptr)
    , m_pVBoxDataType(nullptr)
    , m_pCharChkBox(nullptr)
    , m_pShortChkBox(nullptr)
    , m_pIntChkBox(nullptr)
    , m_pLongChkBox(nullptr)
    , m_pLongLongChkBox(nullptr)
    , m_pRepresentationBinaryOctHexDecGrp(nullptr)
    , m_pRepresentationBinaryOctHexDecBtnsGrp(nullptr)
    , m_pVBoxRepresentation(nullptr)
    , m_pBinaryChkBox(nullptr)
    , m_pOctChkBox(nullptr)
    , m_pHexChkBox(nullptr)
    , m_pDecimalChkBox(nullptr)
    , m_pRepresentationCharacterAsciiGrp(nullptr)
    , m_pRepresentationCharacterAsciiBtnsGrp(nullptr)
    , m_pVBoxCharRepresentation(nullptr)
    , m_pCharacterChkBox(nullptr)
    , m_pAsciiChkBox(nullptr)
    //
    , m_pRangeBox(nullptr)
    , m_pHBoxRange(nullptr)
    , m_pRangeLabel(nullptr)
    , m_pMinNumberText(nullptr)
    , m_pToLabel(nullptr)
    , m_pMaxNumberText(nullptr)
    // 
    , m_pInputGrp(nullptr)
    , m_pHBoxInputParts(nullptr)
    , m_pInputPrompt(nullptr)
    , m_pInputBox(nullptr)
    , m_pEqualLabel(nullptr)
    , m_pDecialCharDetailBox(nullptr)
    , m_pFormatOutuptDetailBox(nullptr)
    , m_pCopyToClipBoardBtn(nullptr)
    //
    , m_pGridLayout(nullptr)
    , m_pDisplayGrp(nullptr)
    , m_pOctHeaderButton(nullptr)
      // , m_pOctAry
    , m_pBinHeaderLabel(nullptr)
    , m_pBinIndexHeaderLabel(nullptr)
      // , m_pBinVBarAry
      // , m_pBinAry
    , m_pHexHeaderLabel(nullptr)
      // , m_pHexVBarAry
      // , m_pHexAry
    , m_pShowFullBitsChkBox(nullptr)
    //
    , m_pStatusBar(nullptr)
    // Data 
    , m_charBits( sizeof(char) * 8U )
    , m_shortBits( sizeof(short) * 8U )
    , m_intBits( sizeof(int) * 8U)
    , m_longBits( sizeof(long) * 8U)
    , m_longlongBits( sizeof(long long) * 8U )
    , m_baseMap{ 2,8,16,10 }
    , m_original2ColorMode(true)
    , m_byteBin()
    , m_byteOct()
    , m_byteHex()
    , m_byteDecimal()
    , m_strColor1("#FF0000")
    , m_strColor2("#00FF00")
    , m_strColor3("#0000FF")
    , m_strDetailOutput("")
    , m_pickedInfo(0U)
{
    setWindowTitle( tr("Decimal-Hex-Binary Convert"));
    resize(800,600);

    for ( int i = 0; i < sc_octCtrlCnt; ++i )    { m_pOctAry[i] = nullptr; }
    for ( int i = 0; i < sc_binaryCtrlCnt; ++i ) { m_pBinIndexAry[i] = nullptr; m_pBinAry[i] = nullptr;  }
    for ( int i = 0; i < sc_hexCtrlCnt; ++i )    { m_pHexAry[i] = nullptr; }

    for ( int i = 0; i < sc_byteVBarCnt; ++i )   { m_pBinVBarAry[i] = nullptr; m_pHexVBarAry[i] = nullptr; }

    // Fill Map
    m_unprintedTable.clear();
    m_unprintedTable.insert(0, "<NUL> (null)");
    m_unprintedTable.insert(1, "<SOH> (start of headline)");
    m_unprintedTable.insert(2, "<STX> (start of text)");
    m_unprintedTable.insert(3, "<ETX> (end of text)");
    m_unprintedTable.insert(4, "<EOT> (end of transmission)");
    m_unprintedTable.insert(5, "<ENQ> (enquiry)");
    m_unprintedTable.insert(6, "<ACK> (acknowledge)");
    m_unprintedTable.insert(7, "<BEL> (bell)");
    m_unprintedTable.insert(8, "<BS> (backspace)");
    m_unprintedTable.insert(9, "\\t <Tab> (horizontal tab)");
    m_unprintedTable.insert(10, "\\n <LF> (NL line feed, new line)");
    m_unprintedTable.insert(11, "<VT> (vertical tab)");
    m_unprintedTable.insert(12, "<FF> (NP form feed,new page)");
    m_unprintedTable.insert(13, "\\r <CR> (carriage return)");
    m_unprintedTable.insert(14, "<SO> (shift out)");
    m_unprintedTable.insert(15, "<SI> (shift in)");
    m_unprintedTable.insert(16, "<DLE> (data link escaple)");
    m_unprintedTable.insert(17, "<DC1> (device control 1)");
    m_unprintedTable.insert(18, "<DC2> (device control 2)");
    m_unprintedTable.insert(19, "<DC3> (device control 3)");
    m_unprintedTable.insert(20, "<DC4> (device control 4)");
    m_unprintedTable.insert(21, "<NAK> (negative acknowledge)");
    m_unprintedTable.insert(22, "<SYN> (synchronous idle)");
    m_unprintedTable.insert(23, "<ETB> (end of trans. block)");
    m_unprintedTable.insert(24, "<CAN> (cancel)");
    m_unprintedTable.insert(25, "<EM> (end of medium)");
    m_unprintedTable.insert(26, "<SUB> (substitute)");
    m_unprintedTable.insert(27, "<ESC> (escaple)");
    m_unprintedTable.insert(28, "<FS> (file separator)");
    m_unprintedTable.insert(29, "<GS> (group separator)");
    m_unprintedTable.insert(30, "<RS> (record separator)");
    m_unprintedTable.insert(31, "<US> (unit separator)");
    m_unprintedTable.insert(32, "<SPC> (Space)");
    m_unprintedTable.insert(127, "<DEL> (delete)");
}

// virtual
myhardcodewindow::~myhardcodewindow()
{
}

void myhardcodewindow::initUI()
{
    m_pCenterWidget = new QWidget(this);
    m_pVBoxGrpAll = new QVBoxLayout();
    
    // m_pChkBoxGrp = new QGroupBox( tr("Options") );
    m_pChkBoxGrp = new QGroupBox();
    m_pHBoxChkBoxGrp = new QHBoxLayout();
    //
    // unsigned / signed
    //
    m_pUnsignedSignedGrp = new QGroupBox( tr("Unsigned / Signed") );
    m_pUnsignedSignedBtnsGrp = new QButtonGroup(m_pUnsignedSignedGrp);
    m_pVBoxUnsignedSigned = new QVBoxLayout();
      m_pUnsignedChkBox  = new QRadioButton( tr("unsigned"));
      m_pSignedChkBox    = new QRadioButton( tr("signed"));

      m_pVBoxUnsignedSigned->addWidget( m_pUnsignedChkBox);
      m_pVBoxUnsignedSigned->addWidget( m_pSignedChkBox);

      m_pUnsignedSignedBtnsGrp->addButton(m_pUnsignedChkBox, 0);
      m_pUnsignedSignedBtnsGrp->addButton(m_pSignedChkBox,  1);
    m_pUnsignedSignedGrp->setLayout( m_pVBoxUnsignedSigned);

    //
    // char / short / int / long / long long
    //
    m_pCharShortIntLongLLGrp = new QGroupBox( tr("Data Type") );
    m_pCharShortIntLongLLBtnsGrp = new QButtonGroup(m_pCharShortIntLongLLGrp);
    m_pVBoxDataType = new QVBoxLayout();
      m_pCharChkBox  = new QRadioButton( tr("char") + QString(" ( %1 bits )").arg(m_charBits) );
      m_pShortChkBox = new QRadioButton( tr("short") + QString(" ( %1 bits )").arg(m_shortBits) );
      m_pIntChkBox   = new QRadioButton( tr("int") + QString(" ( %1 bits )").arg(m_intBits) );
      m_pLongChkBox   = new QRadioButton( tr("long") + QString(" ( %1 bits )").arg(m_longBits) );
      m_pLongLongChkBox   = new QRadioButton( tr("long long") + QString(" ( %1 bits )").arg(m_longlongBits) );

      m_pVBoxDataType->addWidget(m_pCharChkBox);
      m_pVBoxDataType->addWidget(m_pShortChkBox);
      m_pVBoxDataType->addWidget(m_pIntChkBox);
      m_pVBoxDataType->addWidget(m_pLongChkBox);
      m_pVBoxDataType->addWidget(m_pLongLongChkBox);

      m_pCharShortIntLongLLBtnsGrp->addButton(m_pCharChkBox, 0);
      m_pCharShortIntLongLLBtnsGrp->addButton(m_pShortChkBox, 1);
      m_pCharShortIntLongLLBtnsGrp->addButton(m_pIntChkBox, 2);
      m_pCharShortIntLongLLBtnsGrp->addButton(m_pLongChkBox, 3);
      m_pCharShortIntLongLLBtnsGrp->addButton(m_pLongLongChkBox, 4);
    m_pCharShortIntLongLLGrp->setLayout(m_pVBoxDataType);
    //
    // Base Radix : Binary / Octal / Hex / Decimal
    //
    m_pRepresentationBinaryOctHexDecGrp = new QGroupBox( tr("Base") );
    m_pRepresentationBinaryOctHexDecBtnsGrp = new QButtonGroup(m_pRepresentationBinaryOctHexDecGrp);
    m_pVBoxRepresentation = new QVBoxLayout();
      m_pBinaryChkBox = new QRadioButton( tr("Binary (2)") );
      m_pOctChkBox = new QRadioButton( tr("Octal (8)") );
      m_pHexChkBox = new QRadioButton( tr("Hex (16)") );
      m_pDecimalChkBox = new QRadioButton( tr("Decimal (10)") );

      m_pVBoxRepresentation->addWidget(m_pBinaryChkBox);
      m_pVBoxRepresentation->addWidget(m_pOctChkBox);
      m_pVBoxRepresentation->addWidget(m_pHexChkBox);
      m_pVBoxRepresentation->addWidget(m_pDecimalChkBox);

      m_pRepresentationBinaryOctHexDecBtnsGrp->addButton(m_pBinaryChkBox, 0 );
      m_pRepresentationBinaryOctHexDecBtnsGrp->addButton(m_pOctChkBox, 1 );
      m_pRepresentationBinaryOctHexDecBtnsGrp->addButton(m_pHexChkBox, 2 );
      m_pRepresentationBinaryOctHexDecBtnsGrp->addButton(m_pDecimalChkBox, 3 );
    m_pRepresentationBinaryOctHexDecGrp->setLayout(m_pVBoxRepresentation);
    //
    // Char : Character / Ascii(10)
    //
    m_pRepresentationCharacterAsciiGrp = new QGroupBox( tr("Char Style") );
    m_pRepresentationCharacterAsciiBtnsGrp = new QButtonGroup(m_pRepresentationCharacterAsciiGrp);
    m_pVBoxCharRepresentation = new QVBoxLayout();
      m_pCharacterChkBox = new QRadioButton( tr("Character") );
      m_pAsciiChkBox = new QRadioButton( tr("Ascii Code (Based on 10)") );

      m_pVBoxCharRepresentation->addWidget(m_pCharacterChkBox);
      m_pVBoxCharRepresentation->addWidget(m_pAsciiChkBox);

      m_pRepresentationCharacterAsciiBtnsGrp->addButton(m_pCharacterChkBox, 0);
      m_pRepresentationCharacterAsciiBtnsGrp->addButton(m_pAsciiChkBox, 1);
    m_pRepresentationCharacterAsciiGrp->setLayout(m_pVBoxCharRepresentation);

    // Add 4 parts
    m_pHBoxChkBoxGrp->addWidget(m_pUnsignedSignedGrp);
    m_pHBoxChkBoxGrp->addWidget(m_pCharShortIntLongLLGrp);
    m_pHBoxChkBoxGrp->addWidget(m_pRepresentationBinaryOctHexDecGrp);
    m_pHBoxChkBoxGrp->addWidget(m_pRepresentationCharacterAsciiGrp);
    m_pRepresentationCharacterAsciiGrp->hide();
    m_pChkBoxGrp->setLayout(m_pHBoxChkBoxGrp);

    //
    // Range Part
    //
    m_pRangeBox = new QGroupBox( tr("Range") );
    m_pHBoxRange = new QHBoxLayout();
    m_pRangeLabel = new QLabel( tr(" Range From : ") );
    m_pMinNumberText = new QLineEdit( tr("0") );
    m_pMinNumberText->setAlignment( Qt::AlignRight );
    m_pMinNumberText->setReadOnly(true);
    m_pToLabel = new QLabel( tr(" ~ ") );
    m_pMaxNumberText = new QLineEdit( tr("0") );
    m_pMaxNumberText->setAlignment( Qt::AlignLeft );
    m_pMaxNumberText->setReadOnly(true);
    m_pHBoxRange->addWidget( m_pRangeLabel );
    m_pHBoxRange->addWidget( m_pMinNumberText );
    m_pHBoxRange->addWidget( m_pToLabel  );
    m_pHBoxRange->addWidget( m_pMaxNumberText );
    m_pRangeBox->setLayout( m_pHBoxRange );

    // Input Part
    m_pInputGrp = new QGroupBox( tr("Input : ") );
    m_pHBoxInputParts = new QHBoxLayout();
    m_pInputPrompt = new QLabel( tr("Number : ") );
    m_pInputBox = new QLineEdit();
    m_pInputBox->setAlignment( Qt::AlignLeft );
    m_pEqualLabel = new QLabel( tr(" = ") );
    m_pDecialCharDetailBox = new QLineEdit( tr("") );
    m_pDecialCharDetailBox->setReadOnly(true);
    m_pFormatOutuptDetailBox = new QCheckBox( tr("Format Output Detail") );
    m_pFormatOutuptDetailBox->setCheckState( Qt::Unchecked);
    m_pCopyToClipBoardBtn = new QPushButton( tr("Copy to ClipBoard") );
    m_pHBoxInputParts->addWidget(m_pInputPrompt);
    m_pHBoxInputParts->addWidget(m_pInputBox);
    m_pHBoxInputParts->addWidget( m_pEqualLabel );
    m_pHBoxInputParts->addWidget(m_pDecialCharDetailBox);
    m_pHBoxInputParts->addWidget(m_pFormatOutuptDetailBox);
    m_pHBoxInputParts->addWidget(m_pCopyToClipBoardBtn);
    m_pInputGrp->setLayout(m_pHBoxInputParts);
    
    // Grid Part
    m_pGridLayout = new QGridLayout();
    m_pDisplayGrp = new QGroupBox( tr("Display") );
    
    // Octal 
    m_pOctHeaderButton = new QPushButton( tr("Octal   (0): ") );
    m_pOctHeaderButton->setStyleSheet("QPushButton { text-align: left; }");
    for ( int i = 0; i < sc_octCtrlCnt; ++i ) { 
        // m_pOctAry[i] = new QLabel( QString("<font color='%1'>%2</font>").arg( (i%3==0) ? m_strColor1 : (i%3==1 ? m_strColor3 : m_strColor2)).arg("7") );
        m_pOctAry[i] = new QLabel( tr("-") );

        QFont newFnt( m_pOctAry[i]->font() );
        newFnt.setPointSize( newFnt.pointSize() * 3);
        m_pOctAry[i]->setFont(newFnt);
    }

    // Binary
    m_pBinHeaderLabel = new QLabel( tr("Binary      : ") );
    for ( int i = 0; i < sc_byteVBarCnt; ++i ) { 
        m_pBinVBarAry[i] = new QLabel("|"); 

        QFont newFnt( m_pBinVBarAry[i]->font() );
        newFnt.setBold(true);
        newFnt.setPointSize( newFnt.pointSize() * 2);
        m_pBinVBarAry[i]->setFont(newFnt);
    }

    m_pBinIndexHeaderLabel = new QLabel( tr("Binary Index: ") );
    for ( int i = 0; i < sc_binaryCtrlCnt; ++i ) { 
        m_pBinIndexAry[i] = new QLabel( QString("%1").arg(sc_binaryCtrlCnt-i-1) );
        m_pBinAry[i] = new QLabel( QString("<font color='%1'>%2</font>").arg( (i/4%2) == 0 ? m_strColor3 :  m_strColor1).arg("-") );

        QFont idxNewFnt( m_pBinIndexAry[i]->font() );
        idxNewFnt.setPointSizeF( idxNewFnt.pointSizeF() * 0.7f );
        m_pBinIndexAry[i]->setFont( idxNewFnt );
    }
    
    //
    // Hex
    //
    m_pHexHeaderLabel = new QLabel( tr("Hex    (0x): ") );
    for ( int i = 0; i < sc_byteVBarCnt; ++i ) { 
        m_pHexVBarAry[i] = new QLabel("|"); 

        QFont newFnt( m_pHexVBarAry[i]->font() );
        newFnt.setBold(true);
        newFnt.setPointSize( newFnt.pointSize() * 2);
        m_pHexVBarAry[i]->setFont(newFnt);
    }
    for ( int i = 0; i < sc_hexCtrlCnt;  ++i ) { 
        // m_pHexAry[i] = new QLabel("F");
        m_pHexAry[i] = new QLabel( QString("<font color='%1'>%2</font>").arg( i%2 == 0 ? m_strColor3 : m_strColor1 ).arg("-") );
        m_pHexAry[i]->setAlignment(Qt::AlignRight);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Set Grid Detail Layout
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    int rowIdx = 0;
    int j = 0;
    int idx = 1;
    // push all into layout
    //     Oct
    m_pGridLayout->addWidget(m_pOctHeaderButton, rowIdx, 0);
    // | xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx | xxxx xxxx |
    //    1   0  9     8   7     6  5   4     3   2  1     0   9     8  7   6     5   4  3     2   1    
    m_pGridLayout->addWidget(m_pOctAry[0], rowIdx, 1, 1, 2);
    m_pOctAry[0]->setAlignment( Qt::AlignRight );
    idx = 3; // 1+2
    j = 1;
    for ( int i = 1; i < sc_octCtrlCnt; ++i ) {
        int oldGrp = j / 8;
        int newGrp = (j + 3) / 8;

        int columnSpan = newGrp > oldGrp ? 4 : 3;
        // m_pOctAry[i]->setAlignment( Qt::AlignRight );
        m_pGridLayout->addWidget(m_pOctAry[i],rowIdx, idx, 1, columnSpan);
        idx += columnSpan;
        j += 3;
    }

    //
    //     Binary
    //

    // Bits Index Row
    rowIdx = 1;
    idx = 1;
    m_pGridLayout->addWidget(m_pBinIndexHeaderLabel, rowIdx,0);
    for ( int i = 0; i < sc_binaryCtrlCnt; ++i ) {
        if ( i % 8 == 0 ) {
            ++idx;
        }
        m_pGridLayout->addWidget(m_pBinIndexAry[i], rowIdx, idx++);
    }

    // All Bits
    rowIdx = 2;
    j = 0;
    idx = 1;
    m_pGridLayout->addWidget(m_pBinHeaderLabel, rowIdx, 0);
    for ( int i = 0; i < sc_binaryCtrlCnt; ++i ) {
        if ( i % 8 == 0 ) {
            m_pGridLayout->addWidget(m_pBinVBarAry[j++], rowIdx, idx++);
        }
        m_pGridLayout->addWidget(m_pBinAry[i], rowIdx, idx++);
    }
    m_pGridLayout->addWidget(m_pBinVBarAry[sc_byteVBarCnt-1], rowIdx, idx); // push last "|"

    rowIdx = 3;
    //
    //      Hex
    //
    m_pGridLayout->addWidget( m_pHexHeaderLabel, rowIdx, 0);
    j = 0;
    idx = 1;
    for ( int i = 0; i < sc_hexCtrlCnt; ++i ) { // sc_hexCtrlCnt == 16
        if ( i % 2 == 0 ) {
            m_pGridLayout->addWidget(m_pHexVBarAry[j++], rowIdx, idx++);
        }
        m_pGridLayout->addWidget(m_pHexAry[i], rowIdx, idx, 1, 4); // 1 means : 1 row span ;  4 means : 1 Hex Number <==> 4 binary bits ( 4 column spans)
        if ( m_pHexAry[i]!=nullptr ) {
            QFont newfnt( m_pHexAry[i]->font() );
            newfnt.setPointSize( newfnt.pointSize() * 4);
            m_pHexAry[i]->setFont(newfnt);
        }
        idx += 4;
    }
    m_pGridLayout->addWidget(m_pHexVBarAry[sc_byteVBarCnt-1], rowIdx, idx);

    rowIdx = 4;
    m_pShowFullBitsChkBox = new QCheckBox( tr("Show Full Bits") );
    m_pShowFullBitsChkBox->setCheckState( Qt::Checked );
    m_pGridLayout->addWidget( m_pShowFullBitsChkBox, rowIdx , 0, 1, 10);

    m_pDisplayGrp->setLayout(m_pGridLayout);

    m_pVBoxGrpAll->addWidget( m_pChkBoxGrp);
    m_pVBoxGrpAll->addWidget( m_pRangeBox);
    m_pVBoxGrpAll->addWidget( m_pInputGrp);
    m_pVBoxGrpAll->addWidget( m_pDisplayGrp);

    m_pCenterWidget->setLayout(m_pVBoxGrpAll);

    setCentralWidget(m_pCenterWidget);

    m_pStatusBar = new QStatusBar(this);
    // m_pStatusBar->showMessage( tr("Hello DHB Ver 2.0"), 0 );
    // m_pStatusBar->resize( ... );
    setStatusBar( m_pStatusBar);

    // init UI and flag's value
    m_pSignedChkBox->setChecked(true);
    m_pIntChkBox->setChecked( true);
    m_pDecimalChkBox->setChecked( true);
    m_pCharacterChkBox->setChecked( true);
    m_pickedInfo = 0x35U;

    updateDataTypeInfo();
    updateInputPromptInfo();
    doBinaryConvert();

    // m_pInputBox->setPlaceholderText( ( tr("Please input a Decimal Number only contains [0-9] ") ) );

    //
    // finally bind signal and slot
    //
    bindSignalAndSlot();
}


void myhardcodewindow::bindSignalAndSlot()
{
    connect(m_pUnsignedSignedBtnsGrp, SIGNAL(buttonToggled(int,bool)), this, SLOT(onUnsignedSignedClicked(int,bool)) );
    connect(m_pCharShortIntLongLLBtnsGrp,  SIGNAL(buttonToggled(int,bool)), this, SLOT(onDataTypeClicked(int,bool)) );
    connect(m_pRepresentationBinaryOctHexDecBtnsGrp,  SIGNAL(buttonToggled(int,bool)), this, SLOT(onRepresentationClick(int,bool)) );
    connect(m_pRepresentationCharacterAsciiBtnsGrp,   SIGNAL(buttonToggled(int,bool)), this, SLOT(onCharRepresentationClick(int,bool)) );
    connect(m_pOctHeaderButton, SIGNAL(clicked(bool)), this ,SLOT(onOctalSwitchBtnClicked(bool)) );
    connect(m_pInputBox, SIGNAL(returnPressed()) , this , SLOT(doBinaryConvert()) );
    connect(m_pFormatOutuptDetailBox, SIGNAL(stateChanged(int)) , this, SLOT(updateDetailOutput()) );
    connect(m_pShowFullBitsChkBox, SIGNAL(stateChanged(int)), this, SLOT(doBinaryConvert()) );
    connect(m_pCopyToClipBoardBtn, SIGNAL(clicked(bool)), this , SLOT(onCopyBtnClicked(bool)) );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Slot Part
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void myhardcodewindow::onUnsignedSignedClicked(int btnId, bool checkFlag)
{
    static const unsigned int maskBit0 = (0x1U << 0);
    // qDebug() << "unsigned / signed changed";
    // bit index [0]
    if ( checkFlag ) {
        if ( btnId == 1 ) {
            m_pickedInfo |= maskBit0;
        } else {
            m_pickedInfo &= (~maskBit0);
        }

        // update min ~ max and display data type
        updateDataTypeInfo();
        updateInputPromptInfo();

        // qDebug("flag = 0x%x", m_pickedInfo);
        // flagAuxInfo();
    }
}

void myhardcodewindow::onDataTypeClicked(int btnId, bool checkFlag)
{
    // bit [1] ~ [3]
    // qDebug() << "Data Type changed";
    if ( checkFlag ) {
        // qDebug() << "char/short/int/long/long long = " << btnId;
        // btnId : 0 ~ 4
        unsigned int bits3 = static_cast<unsigned int>(btnId);
        bits3 <<= 1;
        m_pickedInfo &= (~0xEU);
        m_pickedInfo |= bits3;

        // update min ~ max and display data type
        updateDataTypeInfo();
        updateInputPromptInfo();

        // qDebug("flag = 0x%x", m_pickedInfo);
        // flagAuxInfo();
    }
}

void myhardcodewindow::onRepresentationClick(int btnId, bool checkFlag)
{
    // bit [4] ~ [5]
    // qDebug() << "2/8/16/10 changed";
    if ( checkFlag ) {
        // qDebug() << "2/8/16/10  = " << btnId;
        unsigned int bits2 = static_cast<unsigned int>(btnId);
        bits2 <<= 4;
        m_pickedInfo &= (~0x30U);
        m_pickedInfo |= bits2;

        updateInputPromptInfo();
        // qDebug("flag = 0x%x", m_pickedInfo);
        // flagAuxInfo();
    }
}

void myhardcodewindow::onCharRepresentationClick(int btnId, bool checkFlag)
{
    static const unsigned int maskBit6 = (0x1U << 6);
    // qDebug() << "Letter/Ascii changed";
    // bit [6]
    if ( checkFlag ) {
        // qDebug() << "Char/Ascii  = " << btnId;

        if ( btnId == 1 ) {
            m_pickedInfo |= maskBit6;
        } else {
            m_pickedInfo &= (~maskBit6);
        }
        updateInputPromptInfo();


        // qDebug("flag = 0x%x", m_pickedInfo);
        // flagAuxInfo();
        // qDebug() << "flag = 0x" << std::hex << m_pickedInfo;
    }
}

void myhardcodewindow::onOctalSwitchBtnClicked(bool check)
{
    Q_UNUSED(check);
    m_original2ColorMode = !m_original2ColorMode;
    updateOctBinaryColor();
}


void myhardcodewindow::updateOctBinaryColor()
{
    auto dt = getDataType();
    int bits = static_cast<int>(m_intBits);
    switch( dt )
    {
    case 0:
        bits = m_charBits;
        break;
    case 1:
        bits = m_shortBits;
        break;
    case 2:
        bits = m_intBits;
        break;
    case 3:
        bits = m_longBits;
        break;
    case 4:
        bits = m_longlongBits;
        break;
    default:
        break;
    }

    innerBitsUpdate(bits);
}

QString myhardcodewindow::flagAuxInfo()
{
    auto us = getUnsignedSigned();
    auto dataType = getDataType();
    auto displayType = getPresentStyle();
    auto la = getCharPresentStyle();
    QString strUs = (us==1 ? "signed" : "unsigned");
    QString strDt;
    switch( dataType ) {
    case 0:
        strDt = "char";
        break;
    case 1:
        strDt = "short";
        break;
    case 2:
        strDt = "int";
        break;
    case 3:
        strDt = "long";
        break;
    case 4:
        strDt = "long long";
        break;
    default:
        break;
    }


    QString strDisplay;
    switch( displayType )
    {
    case 0:
        strDisplay = "Binary";
        break;
    case 1:
        strDisplay = "Octal";
        break;
    case 2:
        strDisplay = "Hex";
        break;
    case 3:
        strDisplay = "Decimal";
        break;
    default:
        break;
    }

    QString strLa = (la==1 ? "Ascii" : "Letter");

    // qDebug() << "U/S = " << strUs  << ","
    //          << "DataType = " << strDt << ","
    //          << "DisplayType = " << strDisplay << ","
    //          << "Char-Type = " << strLa;

    QString retstr = strUs + " " + strDt;
    return retstr;
}



inline 
unsigned int myhardcodewindow::getUnsignedSigned()
{
    return (m_pickedInfo & 0x1U);
}

inline 
unsigned int myhardcodewindow::getDataType()
{
    return ((m_pickedInfo >> 1) & 0x7U);
}


inline 
unsigned int myhardcodewindow::getPresentStyle()
{
    return ((m_pickedInfo >> 4) & 0x3U);
}

inline 
unsigned int myhardcodewindow::getCharPresentStyle()
{
    return ((m_pickedInfo >> 6) & 0x1);
}


void myhardcodewindow::updateDataTypeInfo()
{
    using namespace limitedNumber;

    auto us         = getUnsignedSigned();
    auto dataTypeId = getDataType();
    QString strMin;
    QString strMax;
    QString dataTypeStatuText;
    auto isCharType = false;

    if ( us == 1 ) {
        // signed
        switch ( dataTypeId ) 
        {
        case 0: // char
            {
                strMin.setNum( static_cast<int>(min_signed_char) , 10);
                strMax.setNum( static_cast<int>(max_signed_char) , 10);
                dataTypeStatuText = "signed char => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_charBits / 8U).arg(m_charBits);
                if ( std::numeric_limits<char>::is_signed ) {
                    dataTypeStatuText += ", in your system :    char <==> signed char";
                } else {
                    dataTypeStatuText += ", in your system :    char <==> unsigned char";
                }
                isCharType = true;
            }
            break;
        case 1: // short
            {
                strMin.setNum( min_signed_short, 10);
                strMax.setNum( max_signed_short, 10);
                dataTypeStatuText = "signed short => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_shortBits / 8U).arg(m_shortBits);
            }
            break;
        case 2: // int
            {
                strMin.setNum( min_signed_int, 10);
                strMax.setNum( max_signed_int, 10);
                dataTypeStatuText = "signed int => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_intBits / 8U).arg(m_intBits);
            }
            break;
        case 3: // long
            {
                strMin.setNum( min_signed_long, 10);
                strMax.setNum( max_signed_long, 10);
                dataTypeStatuText = "signed long => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_longBits / 8U).arg(m_longBits);
            }
            break;
        case 4: // long long
            {
                strMin.setNum( min_signed_longlong, 10);
                strMax.setNum( max_signed_longlong, 10);
                dataTypeStatuText = "signed long long => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_longlongBits / 8U).arg(m_longlongBits);
            }
            break;
        default:
            break;
        }
    } else {
        // 0  =>   un-signed
        switch ( dataTypeId ) 
        {
        case 0: // char
            {
                strMin.setNum( static_cast<int>(min_unsigned_char) , 10);
                strMax.setNum( static_cast<int>(max_unsigned_char) , 10);
                dataTypeStatuText = "unsigned char => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_charBits / 8U).arg(m_charBits);
                if ( std::numeric_limits<char>::is_signed ) {
                    dataTypeStatuText += ", in your system :    char <==> signed char";
                } else {
                    dataTypeStatuText += ", in your system :    char <==> unsigned char";
                }
                isCharType = true;
            }
            break;
        case 1: // short
            {
                strMin.setNum( min_unsigned_short, 10);
                strMax.setNum( max_unsigned_short, 10);
                dataTypeStatuText = "unsigned short => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_shortBits / 8U).arg(m_shortBits);
            }
            break;
        case 2: // int
            {
                strMin.setNum( min_unsigned_int, 10);
                strMax.setNum( max_unsigned_int, 10);
                dataTypeStatuText = "unsigned int => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_intBits / 8U).arg(m_intBits);
            }
            break;
        case 3: // long
            {
                strMin.setNum( min_unsigned_long, 10);
                strMax.setNum( max_unsigned_long, 10);
                dataTypeStatuText = "unsigned long => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_longBits / 8U).arg(m_longBits);
            }
            break;
        case 4: // long long
            {
                strMin.setNum( min_unsigned_longlong, 10);
                strMax.setNum( max_unsigned_longlong, 10);
                dataTypeStatuText = "unsigned long long => ";
                dataTypeStatuText += QString("%1 Byte(s) = %2 Bits").arg(m_longlongBits / 8U).arg(m_longlongBits);
            }
            break;
        default:
            break;
        }
        
    }

    if ( isCharType ) {
        m_pRepresentationCharacterAsciiGrp->show();
    } else {
        m_pRepresentationCharacterAsciiGrp->hide();
    }

    m_pMinNumberText->setText(strMin);
    m_pMaxNumberText->setText(strMax);
    if ( !dataTypeStatuText.isEmpty() ) {
        m_pStatusBar->showMessage( dataTypeStatuText, 0 );
    }
}


void myhardcodewindow::updateInputPromptInfo()
{
    auto dt = getDataType();
    auto ps = getPresentStyle();
    auto charPs = getCharPresentStyle();
    if ( dt == 0 ) {
        // char type
        if ( charPs == 0 ) {
            // letter mode
            m_pInputPrompt->setText( tr("Character : ") );
            m_pInputBox->setPlaceholderText( tr("Please input only One character "));
        } else {
            // ascii code (10)
            m_pInputPrompt->setText( tr("Character : ") );
            m_pInputBox->setPlaceholderText( tr("Please input the ascii code (10) of the character "));
        }
    } else {
        // Not char type
        switch ( ps )
        {
        case 0: // Binary
            m_pInputPrompt->setText( tr("Number : (01)") );
            m_pInputBox->setPlaceholderText( tr("Please input a Binary Number only contains [01] "));
            break;
        case 1: // Octal
            m_pInputPrompt->setText( tr("Number : 0") );
            m_pInputBox->setPlaceholderText( tr("Please input an Octal Number only contains [0-7] ") );
            break;
        case 2: // Hex
            m_pInputPrompt->setText( tr("Number : 0x") );
            m_pInputBox->setPlaceholderText( ( tr("Please input a Hex Number only contains [0-9a-fA-F] ") ));
            break;
        case 3: // Decimal
            m_pInputPrompt->setText( tr("Number : ") );
            m_pInputBox->setPlaceholderText( ( tr("Please input a Decimal Number only contains [0-9] ") ) );
            break;
        default:
            break;
        }
    }
}

void myhardcodewindow::doBinaryConvert()
{
    static const QRegExp radix2("\\s*[01]+\\s*");
    static const QRegExp radix8("\\s*[0-7]+\\s*");
    static const QRegExp radix16("\\s*[0-9a-fA-F]+\\s*");
    static const QRegExp radix10("\\s*[+-]?[0-9]+\\s*");
    static const QRegExp* radixAry[] = {
        &radix2,    // #0
        &radix8,    // #1
        &radix16,   // #2
        &radix10    // #3
    };

    static const QString regexDetailAry[] = {
        "[01]+",
        "[0-7]+",
        "[0-9a-fA-F]+",
        "[+-]?[0-9]+",
    };

    static const int arysz = sizeof(radixAry) / sizeof(radixAry[0]);

    m_byteBin.clear();
    m_byteOct.clear();
    m_byteHex.clear();
    m_byteDecimal.clear();
    m_strDetailOutput = "";
    m_original2ColorMode = true;    // restore to original value

    // auto us = getUnsignedSigned();
    auto dt = getDataType();
    auto bohd = getPresentStyle();
    // auto letAscii = getCharPresentStyle();

    QString inputText = m_pInputBox->text();
    if ( dt!=0 && inputText.trimmed().isEmpty() ) {
        return;
    }

    switch ( dt ) 
    {
    case 0: // char
        {
            updateCharBits();
        }
        break;
    case 1: // short
        {
            // m_shortBits
            if ( bohd>=0 && bohd < arysz ) {
                if( !radixAry[bohd]->exactMatch(inputText) ) {
                    m_pStatusBar->showMessage( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]), 0 );
                    m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                    m_pDecialCharDetailBox->setText( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]));
                    setAllBitsDash();
                } else {
                    updateShortBits();
                }
            } else {
                m_pStatusBar->showMessage( tr("Invalid Binary/Oct/Hex/Decimal representation"), 0 );

                m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                m_pDecialCharDetailBox->setText( tr("Invalid Binary/Oct/Hex/Decimal representation") );

                setAllBitsDash();
            }
        }
        break;
    case 2: // int
        {
            if ( bohd>=0 && bohd < arysz ) {
                if( !radixAry[bohd]->exactMatch(inputText) ) {
                    m_pStatusBar->showMessage( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]), 0 );
                    m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                    m_pDecialCharDetailBox->setText( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]));

                    setAllBitsDash();
                } else {
                    updateIntBits();
                }
            } else {
                m_pStatusBar->showMessage( tr("Invalid Binary/Oct/Hex/Decimal representation"), 0 );

                m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                m_pDecialCharDetailBox->setText( tr("Invalid Binary/Oct/Hex/Decimal representation") );

                setAllBitsDash();
            }
        }
        break;
    case 3: // long
        {
            if ( bohd>=0 && bohd < arysz ) {
                if( !radixAry[bohd]->exactMatch(inputText) ) {
                    m_pStatusBar->showMessage( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]), 0 );
                    m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                    m_pDecialCharDetailBox->setText( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]));

                    setAllBitsDash();
                } else {
                    updateLongBits();
                }
            } else {
                m_pStatusBar->showMessage( tr("Invalid Binary/Oct/Hex/Decimal representation"), 0 );

                m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                m_pDecialCharDetailBox->setText( tr("Invalid Binary/Oct/Hex/Decimal representation") );

                setAllBitsDash();
            }
        }
        break;
    case 4: // long long
        {
            if ( bohd>=0 && bohd < arysz ) {
                if( !radixAry[bohd]->exactMatch(inputText) ) {
                    m_pStatusBar->showMessage( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]), 0 );
                    m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                    m_pDecialCharDetailBox->setText( QString("Invalid  Input!!! Format must be %1").arg(regexDetailAry[bohd]));

                    setAllBitsDash();
                } else {
                    updateLongLongBits();
                }
            } else {
                m_pStatusBar->showMessage( tr("Invalid Binary/Oct/Hex/Decimal representation"), 0 );

                m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
                m_pDecialCharDetailBox->setText( tr("Invalid Binary/Oct/Hex/Decimal representation") );

                setAllBitsDash();
            }
        }
        break;
    default:
        break;
    }
}


void myhardcodewindow::updateCharBits()
{
    using namespace limitedNumber;

    QString inputText = m_pInputBox->text();
    if ( inputText.isEmpty() ) {
        m_pDecialCharDetailBox->setStyleSheet("");
        m_pDecialCharDetailBox->setText( tr("") );
        setAllBitsDash();
        return;
    }

    auto us = getUnsignedSigned();
    auto isFullBitChecked = (m_pShowFullBitsChkBox->checkState() == Qt::Checked);
    auto letAscii = getCharPresentStyle();
    auto truncateFlag = true;
    auto succ = false;
    signed short code = 0;

    static const int mapSz1 = 8;
    static const int mapSz2 = 2;
    static const char mapedOctAry[mapSz1][mapSz2] = {
        { '0' , '0' },
        { '1' , '1' },
        { '2' , '2' },
        { '3' , '3' },
        { '4' , '0' },
        { '5' , '1' },
        { '6' , '2' },
        { '7' , '3' }
    };

    if ( letAscii == 0 ) {
        // Single Letter Mode
        if ( inputText.size() == 1 && ((code = static_cast<signed short>(inputText.at(0).unicode())) <= 127 ) ) {
            succ = true;
        } 
    } else {
        // Ascii Code Mode (Only Support Base 10)
        auto convertRet = false;
        code = inputText.toShort(&convertRet, 10);
        if ( convertRet ) {
            if ( us == 0 ) {
                // unsigned char
                succ = (code >= static_cast<int>(min_unsigned_char) &&  code <= static_cast<int>(max_unsigned_char));
            } else {
                //   signed char
                succ = (code >= static_cast<int>(min_signed_char) &&  code <= static_cast<int>(max_signed_char));
            }
        }
    }

    if ( !succ ) {
        if ( letAscii == 0 ) {
            m_pStatusBar->showMessage( QString("Invalid  Input!!! Single Letter Only"), 0 );
            m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
            m_pDecialCharDetailBox->setText( QString("Invalid  Input!!! Single Letter Only") );
            setAllBitsDash();
        } else {
            convertFailedHints();
        }
        return;
    }

    //
    // do real convert
    //
    m_byteBin.setNum(code, 2);
    m_byteOct.setNum(code, 8);
    m_byteHex.setNum(code, 16);
    m_byteDecimal.setNum(code,10);

    // Truncate Unused Bits
    if ( m_byteBin.size() > m_charBits ) {
        m_byteBin = m_byteBin.right(m_charBits);
    }

    int octMaxBits = (m_charBits/3 + 1);
    if ( m_byteOct.size() > octMaxBits  ) {
        m_byteOct = m_byteOct.right(octMaxBits);
    }

    int hexMaxBits = m_charBits/4;
    if ( m_byteHex.size() > hexMaxBits ) {
        m_byteHex = m_byteHex.right(hexMaxBits);
    }

    if ( isFullBitChecked ) {
        m_byteBin = m_byteBin.rightJustified(m_charBits,  '0' , truncateFlag);
        m_byteOct = m_byteOct.rightJustified(m_charBits/3 + 1, '0', truncateFlag);
        m_byteHex = m_byteHex.rightJustified(m_charBits/4, '0', truncateFlag);
    }
    m_byteHex = m_byteHex.toUpper();
    if ( m_byteOct.size() == 3 ) {
        // Special Process Octal Mode Bytes , mapedOctAry
        auto found = false;
        int foundIdx = 0;
        for ( int i = 0; i < mapSz1; ++i ) {
            if ( m_byteOct[0] == mapedOctAry[i][0] ) {
                foundIdx = i;
                found = true;
                break;
            }
        }
        
        if ( found ) {
            m_byteOct[0] = mapedOctAry[foundIdx][1];
        }
    }

    innerBitsUpdate(m_charBits);
}


void myhardcodewindow::updateShortBits()
{
    auto isFullBitChecked = (m_pShowFullBitsChkBox->checkState() == Qt::Checked);

    QString inputText = m_pInputBox->text();
    auto us = getUnsignedSigned();
    auto bohd = getPresentStyle();
    auto convertRet = false;
    if ( us == 0 ) {
        // unsigned short
        unsigned short us_num = inputText.toUShort(&convertRet, m_baseMap[bohd]);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        }
        // else
        m_byteBin.setNum(us_num, 2);
        m_byteOct.setNum(us_num, 8);
        m_byteHex.setNum(us_num, 16);
        m_byteDecimal.setNum(us_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_shortBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_shortBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_shortBits/4, '0');
        }
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_shortBits);
    } else {
        //   signed short
        signed short ss_num = inputText.toShort(&convertRet, m_baseMap[bohd]);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        } 
        // else
        m_byteBin.setNum(ss_num, 2);
        m_byteOct.setNum(ss_num, 8);
        m_byteHex.setNum(ss_num, 16);
        m_byteDecimal.setNum(ss_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_shortBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_shortBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_shortBits/4, '0');
        }
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_shortBits);
    }
}

void myhardcodewindow::updateIntBits()
{
    auto isFullBitChecked = (m_pShowFullBitsChkBox->checkState() == Qt::Checked);

    QString inputText = m_pInputBox->text();
    auto us = getUnsignedSigned();
    auto bohd = getPresentStyle();
    auto convertRet = false;
    // m_intBits
    if ( us == 0 ) {
        // unsigned int
        unsigned int ui_num = inputText.toUInt(&convertRet, m_baseMap[bohd]);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        } 
        // else
        m_byteBin.setNum(ui_num, 2);
        m_byteOct.setNum(ui_num, 8);
        m_byteHex.setNum(ui_num, 16);
        m_byteDecimal.setNum(ui_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_intBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_intBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_intBits/4, '0');
        }
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_intBits);
    } else {
        //   signed int
        signed int si_num = inputText.toInt(&convertRet, m_baseMap[bohd]);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        } 
        // else
        m_byteBin.setNum(si_num, 2);
        m_byteOct.setNum(si_num, 8);
        m_byteHex.setNum(si_num, 16);
        m_byteDecimal.setNum(si_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_intBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_intBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_intBits/4, '0');
        }
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_intBits);
    }
}

void myhardcodewindow::updateLongBits()
{
    if ( m_longBits == m_intBits ) {
        updateIntBits();
    } else if ( m_longBits == m_longlongBits ) {
        updateLongLongBits();
    }
}

void myhardcodewindow::updateLongLongBits()
{
    auto isFullBitChecked = (m_pShowFullBitsChkBox->checkState() == Qt::Checked);

    QString inputText = m_pInputBox->text();
    auto us = getUnsignedSigned();
    auto bohd = getPresentStyle();
    auto convertRet = false;
    if ( us == 0 ) {
        // unsigned long long
        unsigned long long ull_num = inputText.toULongLong(&convertRet, m_baseMap[bohd]);
        // qulonglong qull_num = static_cast<qulonglong>(ull_num);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        } 
        // else
        m_byteBin.setNum(ull_num, 2);
        m_byteOct.setNum(ull_num, 8);
        m_byteHex.setNum(ull_num, 16);
        m_byteDecimal.setNum(ull_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_longlongBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_longlongBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_longlongBits/4, '0');
        } 
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_longlongBits);
    } else {
        //   signed long long
        signed long long sll_num = inputText.toLongLong(&convertRet, m_baseMap[bohd]);
        // qlonglong qll_num = static_cast<qlonglong>(sll_num);
        if ( !convertRet ) {
            convertFailedHints();
            return;
        }
        // else
        m_byteBin.setNum(sll_num, 2);
        m_byteOct.setNum(sll_num, 8);
        m_byteHex.setNum(sll_num, 16);
        m_byteDecimal.setNum(sll_num,10);
        if ( isFullBitChecked ) {
            m_byteBin = m_byteBin.rightJustified(m_longlongBits,  '0');
            m_byteOct = m_byteOct.rightJustified(m_longlongBits/3 + 1, '0');
            m_byteHex = m_byteHex.rightJustified(m_longlongBits/4, '0');
        }
        m_byteHex = m_byteHex.toUpper();

        innerBitsUpdate(m_longlongBits);
    }
}


void myhardcodewindow::innerBitsUpdate(int nBits)
{
    static const QString sc_color3Ary[] = {
        m_strColor1,
        m_strColor2,
        m_strColor3
    };


    if ( m_byteBin.isEmpty() ) {
        return;
    }
    
    //
    // Bininary Part
    //
    int extraColorIdx = 0;
    int dataIdx2 = m_byteBin.size() - 1;
    int modNum = m_original2ColorMode ? 4 : 3;
    int colorIdx2 = 0;
    QString colorBin(m_strColor1);
    for ( int idx2 = sc_binaryCtrlCnt-1; idx2>=0;  ) {
        if ( m_pBinAry[idx2] != nullptr && m_pBinIndexAry[idx2]!=nullptr ) {
            if ( dataIdx2 >=0 ) {
                m_pBinAry[idx2]->show();
                QString strCh( QChar(m_byteBin.at(dataIdx2)) );
                m_pBinAry[idx2]->setText( QString("<font color='%1'>%2</font>").arg(colorBin).arg(strCh) );

                m_pBinIndexAry[idx2]->show();
                m_pBinIndexAry[idx2]->setText( QString("%1").arg(extraColorIdx) );
            } else {
                m_pBinAry[idx2]->hide();
                m_pBinIndexAry[idx2]->hide();
            }   
        }

        --idx2; 
        --dataIdx2;
        ++extraColorIdx;
        if ( extraColorIdx % modNum == 0 ) {
            if ( modNum == 3 ) {
                colorBin = sc_color3Ary[++colorIdx2 % 3];
            } else {
                colorBin = (colorBin == m_strColor1 ? m_strColor3 : m_strColor1);
            }
        }
    }

    //
    // Oct Part
    //
    int dataIdx8 = m_byteOct.size() - 1;
    QString colorOct(m_strColor1);
    int colorIdx8 = 0;
    for ( int idx8 = sc_octCtrlCnt-1; idx8>=0;  ) {
        if ( m_pOctAry[idx8] != nullptr ) {
            if ( dataIdx8 >= 0 ) {
                m_pOctAry[idx8]->show();
                QString strCh( QChar(m_byteOct.at(dataIdx8)) );
                m_pOctAry[idx8]->setText( QString("<font color='%1'>%2</font>").arg(colorOct).arg(strCh) );
            } else {
                m_pOctAry[idx8]->hide();
            }
        }

        --idx8;
        --dataIdx8;
        colorOct = sc_color3Ary[++colorIdx8 % 3];
    }

    //
    // Hex Part
    //
    int dataIdx16 = m_byteHex.size() - 1;
    QString colorHex = m_strColor1;
    for ( int idx16 = sc_hexCtrlCnt-1; idx16>=0;  ) {
        if ( m_pHexAry[idx16] != nullptr ) {
            if ( dataIdx16 >= 0 ) {
                m_pHexAry[idx16]->show();
                QString strCh( QChar(m_byteHex.at(dataIdx16)) );
                m_pHexAry[idx16]->setText( QString("<font color='%1'>%2</font>").arg(colorHex).arg(strCh) );
            } else {
                m_pHexAry[idx16]->hide();
            }
        }

        --idx16;
        --dataIdx16;
        colorHex = (colorHex == m_strColor1 ? m_strColor3 : m_strColor1);
    }


    //
    // Set V-Bar
    //
    for ( int i = sc_byteVBarCnt-1; i >=0; --i ) {
        int reverseIdx = 8 - i;
        if ( reverseIdx < 0 ) {
            reverseIdx *= -1;
        }

        if( m_pBinVBarAry[i]!=nullptr && m_pHexVBarAry[i]!=nullptr ) {
            auto isShow = (reverseIdx * 8) <= nBits;
            m_pBinVBarAry[i]->setVisible(isShow);
            m_pHexVBarAry[i]->setVisible(isShow);
        }
    }

    // Update Detail Text Info
    updateDetailOutput();
}


void myhardcodewindow::convertFailedHints()
{
    m_pStatusBar->showMessage( tr("Input is out of range"), 0 );

    m_pDecialCharDetailBox->setStyleSheet("QLineEdit { color : red }");
    m_pDecialCharDetailBox->setText( tr("Input is out of range") );

    setAllBitsDash();
}


void myhardcodewindow::updateDetailOutput()
{
    if ( m_byteBin.isEmpty() ) {
        return;
    }
    
    // auto us = getUnsignedSigned();
    auto dt = getDataType();
    auto needPrettyFormat = (m_pFormatOutuptDetailBox->checkState() == Qt::Checked);
    auto convertRet = false;
    m_pDecialCharDetailBox->setStyleSheet("");
    m_strDetailOutput = "";
    auto strDataType = flagAuxInfo();
    if ( !needPrettyFormat ) {
        if ( dt == 0 ) {
            // char type, insert char before
            QString strNumber(m_byteDecimal);
            int num = strNumber.toInt(&convertRet, 10);
            if ( num < 0 ) {
                m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
            } else {
                if ( num > 127 ) {
                    m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
                } else {
                    auto it = m_unprintedTable.find(num);
                    QString chInfo;
                    if ( it != m_unprintedTable.end() ) {
                        chInfo = it.value();
                    } else {
                        chInfo = QString("%1").arg(QChar(num));
                    }
                    m_pDecialCharDetailBox->setText( QString(" = '%1' = %2(10) = 0x%3(16) = %4(2) = 0%5(8)").arg(chInfo).arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
                    m_strDetailOutput += QString("%1 num = %2;\n").arg(strDataType).arg(chInfo);
                }
            }
        } else {
            m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
        }

        m_strDetailOutput += QString("%1 num = %2;\n").arg(strDataType).arg( QString(m_byteDecimal) );
        m_strDetailOutput += QString("%1 num = 0x%2;\n").arg(strDataType).arg( QString(m_byteHex) );
        m_strDetailOutput += QString("%1 num = %2(2);\n").arg(strDataType).arg( QString(m_byteBin) );
        m_strDetailOutput += QString("%1 num = 0%2;\n").arg(strDataType).arg( QString(m_byteOct) );
    } else {
        int disFlag = 0;
        QString chInfo;

        if ( dt == 0 ) {
            // char type
            QString strNumber(m_byteDecimal);
            int num = strNumber.toInt(&convertRet, 10);
            if ( num < 0 ) {
                // flag = 1;
                // m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
            } else {
                if ( num > 127 ) {
                    // flag = 2;
                    // m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
                } else {
                    auto it = m_unprintedTable.find(num);
                    if ( it != m_unprintedTable.end() ) {
                        chInfo = it.value();
                    } else {
                        chInfo = QString("%1").arg(QChar(num));
                    }

                    disFlag = 1;
                    // m_pDecialCharDetailBox->setText( QString(" = %1 = %2(10) = 0x%3(16) = %4(2) = 0%5(8)").arg(chInfo).arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
                    m_strDetailOutput += QString("%1 num = '%2';\n").arg(strDataType).arg(chInfo);
                }
            }
        } else {
            // flag = 2;
            // m_pDecialCharDetailBox->setText( QString(" = %1(10) = 0x%2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(QString(m_byteHex)).arg(QString(m_byteBin)).arg(QString(m_byteOct))  );
        }
        
        //
        // Hex
        //
        QString fmt16;
        int sz16 = m_byteHex.size();
        auto isOddCnt = (sz16 % 2 == 1);
        auto cnt = 0;
        // begin "|" Bar
        if ( isOddCnt ) {
            fmt16 += (QString("| ") + QString("    "));    // 4 <space> char
            cnt = 1;
        } else {
            fmt16 += "| ";
            cnt = 0;
        }

        for ( int i = 0; i < sz16; ++i  ) {
            QString strCh( m_byteHex.at(i));
            // strCh = strCh.leftJustified(5);
            strCh = strCh.rightJustified(4);
            strCh += " ";
            fmt16 += strCh;

            ++cnt;
            if ( cnt > 0 && cnt % 2 == 0 ) {
                if ( i != (sz16-1) ) {
                    fmt16 += "| ";
                } else {
                    // last char
                    fmt16 += "|";
                }
            }
        }
        // qDebug() << "fmt16 = " << fmt16;

        //
        // Bin
        //
        QString fmt2;
        int sz2 = m_byteBin.size();
        auto canMod = (sz2 % 8 == 0);
        cnt = 0;
        if ( canMod ) {
            fmt2 += "| ";
            cnt = 0;
        } else {
            fmt2 += "| ";
            cnt = 8 - (sz2 % 8);
            for ( int i = 0; i < cnt; ++i ) {
                fmt2 += " ";
                if ( i > 0 && i % 4 == 0 ) {
                    fmt2 += " ";
                }
            }

        }

        for ( int i = 0; i < sz2; ++i ) {
            QString strCh( m_byteBin.at(i));
            fmt2 += strCh;

            ++cnt;
            if ( cnt > 0 ) {
                if ( cnt % 4 == 0 && cnt % 8 != 0 ) {
                    fmt2 += " ";
                } else if ( cnt % 8 == 0 ) {
                    fmt2 += " |";
                    if ( i != sz2-1 ) {
                        fmt2 += " ";
                    }
                }
            }
        }
        // qDebug() << "fmt-2 = " << fmt2;
        if ( disFlag == 1 ) {
            // With  Letter Display
            m_pDecialCharDetailBox->setText( QString(" = '%1' = %2(10) = %3(16) = %4(2) = 0%5(8)").arg(chInfo).arg(QString(m_byteDecimal)).arg(fmt16).arg(QString(fmt2)).arg(QString(m_byteOct))  );
        } else {
            m_pDecialCharDetailBox->setText( QString(" = %1(10) = %2(16) = %3(2) = 0%4(8)").arg(QString(m_byteDecimal)).arg(fmt16).arg(fmt2).arg(QString(m_byteOct))  );
        }

        m_strDetailOutput += QString("%1 num = %2;\n").arg(strDataType).arg( QString(m_byteDecimal) );
        m_strDetailOutput += QString("%1 num = %2;\n").arg(strDataType).arg( QString(fmt16) );
        m_strDetailOutput += QString("%1 num = %2(2);\n").arg(strDataType).arg( QString(fmt2) );
        m_strDetailOutput += QString("%1 num = 0%2;\n").arg(strDataType).arg( QString(m_byteOct) );
    }


}



void myhardcodewindow::setAllBitsDash()
{
    for ( int i = 0; i < sc_octCtrlCnt; ++i ) {
        if ( m_pOctAry[i] != nullptr ) {
            m_pOctAry[i]->setText("-");
        }
    }

    for ( int i = 0; i < sc_binaryCtrlCnt; ++i ) {
        if ( m_pBinAry[i] != nullptr ) {
            m_pBinAry[i]->setText("-");
        }
    }

    for ( int i = 0; i < sc_hexCtrlCnt; ++i ) {
        if ( m_pHexAry[i] != nullptr ) {
            m_pHexAry[i]->setText("-");
        }
    }
}


void myhardcodewindow::onCopyBtnClicked(bool check)
{
    Q_UNUSED(check);

	auto copyBoard = qApp->clipboard();
    if ( copyBoard!=nullptr && !m_strDetailOutput.isEmpty() ) {
        copyBoard->setText(m_strDetailOutput);
        m_pStatusBar->showMessage( tr("Successfully copied to clipboard"), 0 );
    }

}


