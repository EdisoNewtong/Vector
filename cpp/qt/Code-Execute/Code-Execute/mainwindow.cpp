#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "myException.h"
#include "buff.h"
#include "globalDirector.h"
#include "tokenMgr.h"
#include "cmdOptions.h"
#include "variblePool.h"
#include "dataTypeUtil.h"

#include <QDebug>
#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QTextEdit>

#include <QGridLayout>

static auto SG_debugFlag = true;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pPrintSubMenu( nullptr )
    , m_pCfgFile( nullptr ) 
{
    ui->setupUi(this);

    setupLayout();

    initCfgFile();
    initMenuBar();
}

MainWindow::~MainWindow()
{


    delete ui;

    //
    // sync when program is quit
    //
    if ( m_pCfgFile != nullptr ) {
        m_pCfgFile->close();

        delete m_pCfgFile;
        m_pCfgFile = nullptr;
    }
}

void MainWindow::setupLayout()
{
    auto* pGridLayout = new QGridLayout();

    pGridLayout->addWidget( ui->codeEdit,   0, 0 , 7, 5 );
    pGridLayout->addWidget( ui->outputEdit, 7, 0,  3, 5 );
    pGridLayout->addWidget( ui->pushButton, 10, 4,  1, 1 );

    ui->centralwidget->setLayout( pGridLayout );
}



void MainWindow::initMenuBar()
{
    initMenuBar0();
    initMenuBar1();
}


void MainWindow::initMenuBar0()
{
    constexpr size_t cMenuCnt = 7;
    size_t cIndex = 0;
    QAction* pActionAry[cMenuCnt];
    for( cIndex = 0; cIndex < cMenuCnt; ++cIndex ) {
        pActionAry[cIndex] = nullptr;
    }

    cIndex = 0;
    QMenu* menu0 = ui->menuBar->addMenu( "Option" );

    auto act = menu0->addAction("Print Var List"); pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintVaribleFinally() ); act->setData( QVariant(0) );

    auto submenu = menu0->addMenu("Format");
    act = submenu->addAction("Decimal");           pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( CmdOptions::needPrintVarible_10() ); act->setData( QVariant(1) );
    act = submenu->addAction("Hex");               pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( CmdOptions::needPrintVarible_16() ); act->setData( QVariant(2) );
    act = submenu->addAction("Bin");               pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( CmdOptions::needPrintVarible_2()  ); act->setData( QVariant(3) );

    submenu->addSeparator();
    act = submenu->addAction("Octal");             pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( CmdOptions::needPrintVarible_8() );  act->setData( QVariant(4) );
    act = submenu->addAction("Oct-Style (0)");     pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( !CmdOptions::isOctalDefaultStyle()); act->setData( QVariant(5) );

    submenu->setEnabled( CmdOptions::needPrintVaribleFinally() );
    m_pPrintSubMenu = submenu;

    //
    menu0->addSeparator();
	act = menu0->addAction("Use [Mod Algorithm]  when execute  << or >> ");  pActionAry[cIndex++] = act; act->setCheckable( true ); act->setChecked( CmdOptions::needDoBitwiseShift_Mod_Algorithm() ); act->setData( QVariant(6) );
    menu0->addSeparator();

    for( cIndex = 0; cIndex < cMenuCnt; ++cIndex ) {
        connect( pActionAry[cIndex], SIGNAL( toggled(bool) ), this, SLOT(onMenu0ItemTriggered(bool)) );
    }


    act = menu0->addAction("Save all preferences into cfg file");
    connect(act, SIGNAL( triggered(bool) ), this, SLOT( onSaveToCfg(bool) ) );

}


void MainWindow::initMenuBar1()
{
    // 17
    constexpr size_t cMenuCnt = 17;
    size_t cIndex = 0;
    QAction* pActionAry[cMenuCnt];
    for( cIndex = 0; cIndex < cMenuCnt; ++cIndex ) {
        pActionAry[cIndex] = nullptr;
    }

    cIndex = 0;
    QMenu* menu1 = ui->menuBar->addMenu( "Debug" );
    QAction* act = nullptr;
    act = menu1->addAction( "Print Run-Time Warnings" ); pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintParseRuntimeWarning() ); act->setData( QVariant(0) );

    menu1->addSeparator();
    auto submenu = menu1->addMenu("Operator/Operands");
    act = submenu->addAction("Print Operator Stack");                          pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintOperatorStackAll() ); act->setData( QVariant(1) );
    act = submenu->addAction("Print SuffixExp Before");                        pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintSuffixExpressionBefore() ); act->setData( QVariant(2) );
    act = submenu->addAction("Print SuffixExp After Build");                   pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintSuffixExpressionAfterBuild() ); act->setData( QVariant(3) );
    act = submenu->addAction("Print Suffix Expression After Evaluate");        pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintSuffixExpressionAfterEvaluate() ); act->setData( QVariant(4) );
    act = submenu->addAction("Trace OpStack SuffixExp Change");                pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTraceOperatorStackSuffixExpressionChange() ); act->setData( QVariant(5) );
    menu1->addSeparator();

    act = menu1->addAction( "Trace +/- Property Change");                      pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTracePositiveNegativePropertyChange() ); act->setData( QVariant(6) );
    act = menu1->addAction( "Trace TmpExp Process");                           pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTraceTmpExpressionProcess() ); act->setData( QVariant(7) );
    act = menu1->addAction( "Treat signed integer bit shift as warning");      pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTreatSignedIntergerBitShiftAsWarning() ); act->setData( QVariant(9) );
    act = menu1->addAction( "Trace float number zero when do Divide");         pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needCheckFloatNumberZero() ); act->setData( QVariant(10) );
    act = menu1->addAction( "process tmp expression without assignment");      pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needProcessTmpExpressionWithoutAssignment() ); act->setData( QVariant(11) );
    act = menu1->addAction( "treat blank statement as warning");               pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTreatBlankStatementAsWarning() ); act->setData( QVariant(12) );
    act = menu1->addAction( "treat uninitialized varible as error");           pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTreatUninitializedVaribleAsError() ); act->setData( QVariant(13) );
    act = menu1->addAction( "check fixed literal int range when assign");      pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needCheckFixedLiteralIntRangeWhenAssign() ); act->setData( QVariant(15) );

    menu1->addSeparator();
    act = menu1->addAction( "print basic data range");                         pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintDataTypeRange() ); act->setData( QVariant(8) );
    act = menu1->addAction( "print buff size");                                pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needPrintSrcCodeLength() ); act->setData( QVariant(14) );
    act = menu1->addAction( "print parse time step");                          pActionAry[cIndex++] = act; act->setCheckable(true); act->setChecked( CmdOptions::needTraceParseTimeStep() ); act->setData( QVariant(23) );

    for( cIndex = 0; cIndex < cMenuCnt; ++cIndex ) {
        connect( pActionAry[cIndex], SIGNAL( toggled(bool) ), this, SLOT(onMenu1ItemTriggered(bool)) );
    }

}



void MainWindow::onMenu0ItemTriggered(bool checked)
{
    Q_UNUSED(checked)

    QAction* pClickedMenu = qobject_cast<QAction*>( sender() );
    if ( pClickedMenu != nullptr ) {
        bool bCastSuccessful = false;
        auto tag = pClickedMenu->data().toInt(&bCastSuccessful);
        if ( bCastSuccessful ) {
            CmdOptions::toggleFlagBit( static_cast<quint32>(tag) );

            if ( tag == 0  &&  m_pPrintSubMenu != nullptr ) {
                 m_pPrintSubMenu->setEnabled( CmdOptions::needPrintVaribleFinally() );
            }

            // if ( CmdOptions::s_needSaveToCfgAtOnce ) {
            //     saveToFile(false);
            // }
        }
    }
}


void MainWindow::onMenu1ItemTriggered(bool checked)
{
    Q_UNUSED(checked)

    QAction* pClickedMenu = qobject_cast<QAction*>( sender() );
    if ( pClickedMenu != nullptr ) {
        bool bCastSuccessful = false;
        auto tag = pClickedMenu->data().toInt(&bCastSuccessful);
        if ( bCastSuccessful ) {
            CmdOptions::toggleDebugOptionBit( static_cast<quint32>(tag) );

            // if ( CmdOptions::s_needSaveToCfgAtOnce ) {
            //     saveToFile(false);
            // }
        }
    }
}


void MainWindow::on_pushButton_clicked()
{
    de_highlight();

    DataTypeUtil::init();
    VariblePool::init();
    TokenMgr::init();

    // TODO :
    ui->outputEdit->setPlainText( QString("") );

    QString outstr;
    QString codestr = ui->codeEdit->toPlainText();
    QString trimmedstr = codestr.trimmed();
    if ( trimmedstr.isNull() || trimmedstr.isEmpty() ) {
        TokenMgr::release();
        VariblePool::finalize();
        DataTypeUtil::finalize();

        return;
    }

    QByteArray codeba;
    codeba += codestr;
    int baSize = codeba.size();
    if ( CmdOptions::needPrintSrcCodeLength() ) {
        outstr += "\n";   
        outstr += "==================================================\n";
        outstr += QString(" code.length = %1 byte(s) \n").arg(baSize);
        outstr += "==================================================\n";
    }


    outstr += CmdOptions::printDataTypeRangeIfNecessary();

    GlobalDirector director;
    try {
        const char* buf = codeba.constData();
        
        Buff* fileBuff = new Buff(buf,baSize);

        director.setTextEditor( ui->outputEdit );
        director.setData(fileBuff);
        director.doParse();
    } catch( const MyException& e ) { 
        // TODO : high-light
        outstr += QString("[ERROR] : Meet a self-defined exception : %1\n").arg( e.getExceptionDetail().c_str() );
        ChInfo cursor;
        if ( e.hasCursorInfo(&cursor) ) {
            qDebug() << "HighLight cursor : " << cursor.qtCursorIdx;
            highLightCatchedPosition(cursor.qtCursorIdx);
        } else {
             qDebug() << "No Cursor info ";
        }
    } catch(const std::exception& e) {
        outstr += QString("[ERROR] : Meet a common exception : %1\n").arg( e.what() );
    } catch( ... ) {
        outstr += QString("[ERROR] : Meet an unexpection exception.");
    }

    QString oldstr = ui->outputEdit->toPlainText();
    oldstr = (outstr + oldstr);
    ui->outputEdit->setPlainText( oldstr );

    TokenMgr::release();
    VariblePool::finalize();
    DataTypeUtil::finalize();
}


void MainWindow::initCfgFile()
{
    int step = static_cast<int>( sizeof(quint32) );
    m_pCfgFile = new QFile( QString(CmdOptions::getDefaultCfgFileName().c_str()) );
    if ( m_pCfgFile->open(QIODevice::ReadWrite) ) {
        auto bytes = m_pCfgFile->readAll();
        auto nBytesCnt = bytes.size();
        if ( nBytesCnt == (step * 2) ) {
            QDataStream ds(&bytes, QIODevice::ReadOnly);
            quint32 flagBytes = 0u;   
            quint32 optionBytes = 0u;
            ds >> flagBytes >> optionBytes;

            if ( SG_debugFlag ) {
                qDebug() << "1st Read : flag = " << flagBytes << ", debugOption = " << optionBytes;
            }

            CmdOptions::forceSetFlag( flagBytes );
            CmdOptions::forceSetOption( optionBytes );
        }
    }
}


void MainWindow::saveToFile(bool needDelete)
{
    if ( m_pCfgFile != nullptr ) {
        QByteArray ba;
        QDataStream ds(&ba, QIODevice::WriteOnly);
        auto flag = CmdOptions::getFlag();
        auto debugOption = CmdOptions::getDebugOption();
        if ( SG_debugFlag ) {
            qDebug() << "To Write : flag = " << flag << ", debugOption = " << debugOption;
        }
        ds << flag << debugOption;

        m_pCfgFile->reset();
        m_pCfgFile->write( ba );
        m_pCfgFile->flush();

        if ( needDelete ) {
            m_pCfgFile->close();

            delete m_pCfgFile;
            m_pCfgFile = nullptr;
        }
    }
}


void MainWindow::highLightCatchedPosition(int startCursorPos)
{
    QList<QTextEdit::ExtraSelection> lst;

    //
    //  Core
    //
    // 移动当前的光标到指定的位置 , 这个操作会触发 QPlainTextEdit 的滚动条的移动(如果当前的设置的光标位置不在当前的可视区域时，会立即滚动(不带动画的)到设置的位置)
    auto cursor = ui->codeEdit->textCursor();
    QTextCursor currentCursor(cursor);
    currentCursor.setPosition(startCursorPos, QTextCursor::MoveAnchor);

    QTextEdit::ExtraSelection selection;
    cursor.setPosition(startCursorPos, QTextCursor::MoveAnchor);
    cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  1 );

    selection.cursor = cursor;
    selection.format.setBackground( QBrush(Qt::yellow) );
    lst.push_back(selection);

    ui->codeEdit->setExtraSelections(lst);

    ui->codeEdit->setTextCursor(currentCursor);
}


void MainWindow::de_highlight()
{
    QList<QTextEdit::ExtraSelection> lst;
    ui->codeEdit->setExtraSelections(lst);
}


void MainWindow::onSaveToCfg(bool checked)
{
    Q_UNUSED(checked)

    saveToFile(false);

    ui->statusbar->showMessage( QString("Save to \"%1\" Done").arg( QString(CmdOptions::getDefaultCfgFileName().c_str()) ),  2000 );

}