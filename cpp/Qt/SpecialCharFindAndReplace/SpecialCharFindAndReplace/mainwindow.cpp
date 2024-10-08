#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QHash>
#include <QTextEdit>

// static 
const QMap<QString, QString> MainWindow::sc_punctuationMap{
    { "`", "Back-Quote" },  // `  
    { "~", "Tilde"},
    { " ", "Space"},
    { "!", "Exclamation" },
    { "@", "At" },
    { "#", "Hash"},
    { "$", "Dollar" },
    { "%", "Percent" },
    { "^", "Caret" },
    { "&", "And" },
    { "*", "Asterisk" },
    { "(", "Left Parenthesis" },
    { ")", "Right Parenthesis" },
    { "-", "Minus" },
    { "_", "UnderLine" },
    { "=", "Equal" },
    { "+", "Plus" },
    { "[", "Left Bracket" },
    { "{", "Left Brace" },
    { "]", "Right Bracket" },
    { "}", "Right Brace" },
    { "\\", "Back Slash" },
    { "|", "Bar" },
    { ";", "Semicolon" },
    { ":", "Colon" },
    { "'", "Single Quotation" },
    { "\"", "Quotation" },
    { ",", "Comma" },
    { "<", "Less Than" },
    { ".", "Dot" },
    { ">", "Greater Than" },
    { "/", "Slash" },
    { "?", "Question Mark" },
    //////////////////////////////////////////////////

    { "a",     "a" },
    { "b",     "b" },
    { "c",     "c" },
    { "d",     "d" },
    { "e",     "e" },
    { "f",     "f" },
    { "g",     "g" },
    { "h",     "h" },
    { "i",     "i" },
    { "j",     "j" },
    { "k",     "k" },
    { "l",     "l" },
    { "m",     "m" },
    { "n",     "n" },
    { "o",     "o" },
    { "p",     "p" },
    { "q",     "q" },
    { "r",     "r" },
    { "s",     "s" },
    { "t",     "t" },
    { "u",     "u" },
    { "v",     "v" },
    { "w",     "w" },
    { "x",     "x" },
    { "y",     "y" },
    { "z",     "z" },

    //////////////////////////////////////////////////
    { "A",     "A" },
    { "B",     "B" },
    { "C",     "C" },
    { "D",     "D" },
    { "E",     "E" },
    { "F",     "F" },
    { "G",     "G" },
    { "H",     "H" },
    { "I",     "I" },
    { "J",     "J" },
    { "K",     "K" },
    { "L",     "L" },
    { "M",     "M" },
    { "N",     "N" },
    { "O",     "O" },
    { "P",     "P" },
    { "Q",     "Q" },
    { "R",     "R" },
    { "S",     "S" },
    { "T",     "T" },
    { "U",     "U" },
    { "V",     "V" },
    { "W",     "W" },
    { "X",     "X" },
    { "Y",     "Y" },
    { "Z",     "Z" },
    //////////////////////////////////////////////////

    { "0",     "0" },
    { "1",     "1" },
    { "2",     "2" },
    { "3",     "3" },
    { "4",     "4" },
    { "5",     "5" },
    { "6",     "6" },
    { "7",     "7" },
    { "8",     "8" },
    { "9",     "9" }
};

static const QString G_DISPLAYED_SPACE("˽");

static const QString G_DEFAULT_SEPERATOR_SPACE(" ");
static const QString G_DEFAULT_SEPERATOR_COMMA(",");
static const QString G_DEFAULT_SEPERATOR_COLON(":");

static const QChar G_DEFAULT_SEPERATOR_SPACE_CHAR(' ');
static const QChar G_DEFAULT_SEPERATOR_COMMA_CHAR(',');
static const QChar G_DEFAULT_SEPERATOR_COLON_CHAR(':');

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cfgObjectList()
    , m_splitQStringList()
    , m_matchedPairList()
    , m_TextChangedStateAfterScan(0)
    , m_matchedIdx(-1)
{
    ui->setupUi(this);
    initUI();
}

void MainWindow::initUI()
{
    ui->cmdGrp->hide();

    QStringList labels{ QString("Enable"), QString("Tag"), QString("Char"), QString("Variants-Char-List"), QString("Sep"), QString("Detail") };
    ui->cfgTableWidget->setHorizontalHeaderLabels(labels);

    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_ENABLE_STATE), 30);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_TAG_NAME),40);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_ORIGINAL_CHAR), 30);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_VARIANT_CHAR_LIST), 90);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_SEP_CHAR), 30);
    ui->cfgTableWidget->setColumnWidth(static_cast<int>(E_DETAIL_INFO), 80);

    initCfgTable();

    ui->cfgTableWidget->resizeColumnsToContents();

    ui->actionPreviousFind->setEnabled(true);
    ui->actionNextFind->setEnabled(true);

}

void MainWindow::initCfgTable()
{
    /* ˽    <--- Space           */
    QList<QString> cfgList{
        "`,:｀,·", 
        "~,:～", 
        " ,:　",                // space line '˽' , special row that <space> is invisible char
        "!,:！", 
        "@,:＠", 
        "#,:＃", 
        "$,:＄,￥", 
        "%,:％", 
        "^,:＾,……", 
        "&,:＆", 
        "*,:＊,×", 
        "(,:（", 
        "),:）", 
        "-,:－", 
        "_,:＿,——", 
        "=,:＝", 
        "+,:＋", 
        "[,:［,【", 
        "{,:｛", 
        "],:］,】", 
        "},:｝", 
        "\\,:＼,、",
        "|,:｜", 
        ";,:；", 
        ":,:：",               // colon line ':'   , Special row that  1st ':' means the normal char ,  2nd ':' is a seperator 
        "',:＇,‘,’", 
        "\",:＂,“,”",
        ", :，",               // comma line ','   , special row that  Seperator is   '˽' ( Space )    rather than   ',' 
        "<,:＜,《", 
        ".,:．,。", 
        ">,:＞,》", 
        "/,:／", 
        "?,:？", 
        //////////////////////////////////////////////////
        // a ~ z
        "a,:ａ", 
        "b,:ｂ", 
        "c,:ｃ", 
        "d,:ｄ", 
        "e,:ｅ", 
        "f,:ｆ", 
        "g,:ｇ", 
        "h,:ｈ", 
        "i,:ｉ", 
        "j,:ｊ", 
        "k,:ｋ", 
        "l,:ｌ", 
        "m,:ｍ", 
        "n,:ｎ", 
        "o,:ｏ", 
        "p,:ｐ", 
        "q,:ｑ", 
        "r,:ｒ", 
        "s,:ｓ", 
        "t,:ｔ", 
        "u,:ｕ", 
        "v,:ｖ", 
        "w,:ｗ", 
        "x,:ｘ", 
        "y,:ｙ", 
        "z,:ｚ", 
        //////////////////////////////////////////////////
        // A ~ Z
        "A,:Ａ", 
        "B,:Ｂ", 
        "C,:Ｃ", 
        "D,:Ｄ", 
        "E,:Ｅ", 
        "F,:Ｆ", 
        "G,:Ｇ", 
        "H,:Ｈ", 
        "I,:Ｉ", 
        "J,:Ｊ", 
        "K,:Ｋ", 
        "L,:Ｌ", 
        "M,:Ｍ", 
        "N,:Ｎ", 
        "O,:Ｏ", 
        "P,:Ｐ", 
        "Q,:Ｑ", 
        "R,:Ｒ", 
        "S,:Ｓ", 
        "T,:Ｔ", 
        "U,:Ｕ", 
        "V,:Ｖ", 
        "W,:Ｗ", 
        "X,:Ｘ", 
        "Y,:Ｙ", 
        "Z,:Ｚ", 
        //////////////////////////////////////////////////
        // 0 ~ 9
        "0,:０", 
        "1,:１", 
        "2,:２", 
        "3,:３", 
        "4,:４", 
        "5,:５", 
        "6,:６", 
        "7,:７", 
        "8,:８", 
        "9,:９"
    };

    m_cfgObjectList.clear();

    Q_UNUSED(G_DEFAULT_SEPERATOR_SPACE_CHAR)

    const auto fixed_sz = cfgList.size();
    for ( auto i = 0; i < fixed_sz; ++i ) {
        find_replace_object obj;
        auto str_obj = cfgList.at(i);
        if ( !str_obj.isEmpty() ) {
            auto sepColonIdx = -1;
            auto bIsStartWithColon = str_obj.startsWith( G_DEFAULT_SEPERATOR_COLON_CHAR );
            if ( bIsStartWithColon ) {
                // start ':'
                sepColonIdx = str_obj.indexOf( G_DEFAULT_SEPERATOR_COLON_CHAR, 1 );
                if ( sepColonIdx == -1 ) {
                    qDebug() << "1st is ':' , But haven't find 2nd  ':' as a seperator. ";
                } 
            } else {
                sepColonIdx = str_obj.indexOf( G_DEFAULT_SEPERATOR_COLON_CHAR, 0 );
                if ( sepColonIdx == -1 ) {
                    qDebug() << " Haven't find the 1st ':' as a seperator. ";
                } 
            }

            if ( sepColonIdx != -1 ) {
                QStringList sepList;
                if ( !bIsStartWithColon ) {
                    sepList = str_obj.split(G_DEFAULT_SEPERATOR_COLON_CHAR);
                } else {
                    // 1st part is
                    QString str_part1 = str_obj.mid(0, sepColonIdx);
                    QString str_part2 = str_obj.mid(sepColonIdx+1);
                    sepList.push_back( str_part1 );
                    sepList.push_back( str_part2 );
                }
                auto sList_sz = sepList.size();
                if ( sList_sz == 2 ) {
                    auto strPart1 = sepList.at(0);
                    auto strPart2 = sepList.at(1);
                    find_replace_object core_obj;

                    auto meetError = false;
                    auto iCommaIdx = strPart1.indexOf( G_DEFAULT_SEPERATOR_COMMA_CHAR, 0 );
                    QChar realSeperator(G_DEFAULT_SEPERATOR_COMMA_CHAR);
                    if ( iCommaIdx == 0 ) { 
                        // start with ','
                        core_obj.original_ch_Parttern = G_DEFAULT_SEPERATOR_COMMA;
                        auto nextIdx = (iCommaIdx + 1);
                        if ( nextIdx >= strPart1.size() ) {
                            qDebug() << "Haven't find different seperator at special comma   ',' row. ";
                            meetError = true;
                        } else {
                            realSeperator = strPart1.at( nextIdx );
                        }
                    } else {
                        // not start with ','
                        core_obj.original_ch_Parttern = strPart1.mid(0, iCommaIdx);
                    }

                    if ( !meetError ) {
                        core_obj.seperator                    = realSeperator;
                        core_obj.original_ch_replace_Parttern = strPart2;
                        core_obj.variants_ch_Parttern         = strPart2.split( core_obj.seperator );

                        m_cfgObjectList.push_back( core_obj );
                    }
                } else {
                    qDebug() << "Seperate List must contain 2 elements strictly. Now it contains " << sList_sz << " elements. ";
                }
            }
        }
    }

    auto cfgTableRows = m_cfgObjectList.size();
    qDebug() << "cfgTable.size() = " << cfgTableRows;
    for(auto idx = 0; idx < cfgTableRows; ++idx ) {
        const auto& cfgObj = m_cfgObjectList.at(idx);
        ui->cfgTableWidget->insertRow( idx );

        MainWindow::E_HEADER_TAG h_col;

        {
            h_col = MainWindow::E_ENABLE_STATE;
            QTableWidgetItem *newItem = new QTableWidgetItem("", QTableWidgetItem::UserType +  h_col );
            newItem->setData(Qt::CheckStateRole, QVariant(true) );
            newItem->setCheckState(Qt::Checked);
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
        }

        {
            h_col = MainWindow::E_TAG_NAME;
            QString displayed_pattern = cfgObj.original_ch_Parttern;
            QString name_meaning("???");
            auto it = sc_punctuationMap.find(displayed_pattern);
            if ( it != sc_punctuationMap.end() ) {
                name_meaning = it.value();
            }
            
            QTableWidgetItem *newItem = new QTableWidgetItem(name_meaning, QTableWidgetItem::UserType +  h_col );
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
        }

        {
            h_col = MainWindow::E_ORIGINAL_CHAR;

            auto bSpecialFlag = false;
            QString displayed_pattern = cfgObj.original_ch_Parttern;
            if ( !displayed_pattern.isEmpty() ) {
                if ( displayed_pattern.at(0) == G_DEFAULT_SEPERATOR_SPACE_CHAR ) {
                    displayed_pattern = G_DISPLAYED_SPACE;
                    bSpecialFlag = true;
                }
            } 

            QTableWidgetItem *newItem = new QTableWidgetItem( displayed_pattern, QTableWidgetItem::UserType +  h_col );
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
            if ( bSpecialFlag ) {
                newItem->setForeground( QBrush(Qt::red) );
                newItem->setBackground( QBrush(Qt::black) );

                auto largerFont = newItem->font();
                largerFont.setPointSize( largerFont.pointSize() + 5 );
                largerFont.setBold( true );
                newItem->setFont( largerFont );
            }
        }

        {
            h_col = MainWindow::E_VARIANT_CHAR_LIST;
            QTableWidgetItem *newItem = new QTableWidgetItem( cfgObj.original_ch_replace_Parttern  , QTableWidgetItem::UserType +  h_col );
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
        }

        {
            h_col = MainWindow::E_SEP_CHAR;
            QTableWidgetItem *newItem = new QTableWidgetItem( QString(cfgObj.seperator)  , QTableWidgetItem::UserType +  h_col );
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
        }

        {
            h_col = MainWindow::E_DETAIL_INFO;
            QTableWidgetItem *newItem = new QTableWidgetItem( QString(" ... ")  , QTableWidgetItem::UserType +  h_col );
            ui->cfgTableWidget->setItem(idx , static_cast<int>(h_col), newItem);
        }

    }


}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loadFileBtn_clicked()
{
    auto targetText2Scan = ui->targetScanTextBox->toPlainText();
    qDebug() << "inputstring.size() = " << targetText2Scan.size();
    qDebug() << "rep = " << QChar::ReplacementCharacter;
    auto retAllAry = targetText2Scan.toUcs4();
    int idx1 = 0;
    qDebug() << "Total ucs4.size() = " << retAllAry.size();
    for( auto& e : retAllAry ) {
        qDebug() << (idx1++) << ".   " << e << ", "<< (QChar(e) == QChar::ReplacementCharacter ? " [IS] rep" : "is normal");
    }
    qDebug() << "--------------------------------------------------";


    /*
    for( auto iidx = 0; iidx < targetText2Scan.size(); ++iidx) {
        qDebug() << "iidx = " << iidx;
        QString sep_str = targetText2Scan.mid(iidx,1);
        auto retAry = sep_str.toUcs4();
        int idx = 0;
        qDebug() << "ucs4.size() = " << retAry.size();
        for( auto& e : retAry ) {
            qDebug() << (idx++) << ".   " << e << ", "<< (QChar(e) == QChar::ReplacementCharacter ? " [IS] rep" : "is normal") << endl;
        }
        qDebug() << "--------------------------------------------------";
    }
    */


}

void MainWindow::on_saveFileBtn_clicked()
{

}



void MainWindow::on_searchBtn_clicked()
{
    auto startIdx = 0;

    qDebug() << "startIdx = " << startIdx;
    auto bIsOneMatchedStop = false;
    searchMatched(startIdx, bIsOneMatchedStop);
    m_TextChangedStateAfterScan = 1;
    m_matchedIdx = -1;

    if ( !m_matchedPairList.isEmpty() ) {
        qDebug() << "highlight index[0] of " << m_matchedPairList.size();
        m_matchedIdx = 0;
        highLightOneMatched(true);
    } else {
        m_matchedIdx = -1;
        qDebug() << "matched list is empty";
    }
}

void MainWindow::on_replace1Btn_clicked()
{

}

void MainWindow::on_replaceAllBtn_clicked()
{

}

void MainWindow::on_sPrevBtn_clicked()
{
    on_actionPreviousFind_triggered();
}

void MainWindow::on_sNextBtn_clicked()
{
    on_actionNextFind_triggered();
}

void MainWindow::on_newCfgBtn_clicked()
{
    ui->cfgTableWidget->clearContents();
    ui->cfgTableWidget->setRowCount(0);

}

void MainWindow::on_newRowBtn_clicked()
{
    ui->cfgTableWidget->insertRow( ui->cfgTableWidget->rowCount() );
}

void MainWindow::on_loadCfgBtn_clicked()
{
    // virtual QVariant loadResource(int type, const QUrl &name)
    // QVariant var = ui->targetScanTextBox->loadResource( QTextDocument::ImageResource, QUrl( QString("file://%1").arg(path) ) );
    
}

void MainWindow::on_saveCfgBtn_clicked()
{

}

//
// Check Table's internal data logic
//--------------------------------------------------
//     !! Core Core Core !!
//--------------------------------------------------
void MainWindow::on_checkLogicBtn_clicked()
{
    QHash<QString,int> originalChMap;
    QHash<QString, QPair<int,int> > repalcementChMap;

    /*
        key condition :
              1. pick every two rows , the original-char must be different from each other
                 otherwise :

                               * <--＊
                               * <-- ×
                            the previous two lines can be merged into  one config :

                               * <- ＊,×


              2. pick every two rows , each char to be replaced in the search list must keep unique in the whole replacement list

                            a <-- A
                            b <-- A
                        so when an 'A' is existed in a certain place inside text ,
                        what will be the correct replacement ?

                        replace A --> a ?
                     or replace A --> b ?       
                     It's ambiguious


              3. special replacement for cycle condition
                   
                e.g.
                       I:        a <-   A
                      II:        A <-   a

                    The Text is : Abc

                    if we hit 'A' :
                       follow the rule I :  abc
                       follow the rule II : Abc (it will keep unchanged )

                Now lets' change the oreder of 2 rules:

                       I:        A <-   a
                      II:        a <-   A

                    The Text is still : Abc

                    if we hit 'A' :
                       follow the rule I :  No Matched ( keep the text unchanged )
                       follow the rule II : abc <-- Abc  

        
                Conclusion : the replacement list should not contains the keys

            4. 
                a <--  A
                b <--  B
               𝛂𝛃 <-- AB

               so    ab <-- AB
               or    𝛂𝛃 <-- AB

            It's ambiguious

    */
    auto sameKeyPair = qMakePair(-1,-1);
    auto grpPair = qMakePair( qMakePair(-1,-1), qMakePair(-1,-1) );
    auto hasSameKey = false;
    auto hasSameRepCh = false;
    for ( auto i = 0; i < m_cfgObjectList.size(); ++i ) {
        const auto& cfg = m_cfgObjectList.at(i) ;
        auto foundIt = originalChMap.find( cfg.original_ch_Parttern );
        if ( foundIt == originalChMap.end() ) {
            // Not found
            originalChMap.insert( cfg.original_ch_Parttern, i );
            
            auto idx = 0;
            for ( auto j = 0; j < cfg.variants_ch_Parttern.size(); ++j ) {
                const auto& repStr = cfg.variants_ch_Parttern.at(j);
                auto foundRepIt = repalcementChMap.find( repStr );
                if ( foundRepIt == repalcementChMap.end() ) {
                    // Not found
                    repalcementChMap.insert(repStr, qMakePair(i, idx) );
                } else {
                    grpPair = qMakePair( foundRepIt.value(), qMakePair(i,idx) );
                    hasSameRepCh = true;
                    break;
                }
            }

            if ( hasSameRepCh ) {
                break;
            }
        } else {
            sameKeyPair = qMakePair( foundIt.value() , i);
            hasSameKey = true;
            break;
        }
    }

    if ( hasSameKey  ) {
        qDebug() << "Has Same Key";
        return;
    }

    if ( hasSameRepCh ) {
        qDebug() << "Has Same Replacement Char";
        return;
    }

    auto bIsReplacementListContainsKey = false;
    for( auto it = originalChMap.begin(); it!=originalChMap.end(); ++it ) {
        auto foundIt = repalcementChMap.find( it.key() );
        if ( foundIt != repalcementChMap.end() ) {
            bIsReplacementListContainsKey = true;
            break;
        }
    }


    if ( bIsReplacementListContainsKey ) {
        qDebug() << "The exists a key inside the replacement char list";
        return;
    }
}

void MainWindow::on_actionShow_InfoBox_toggled(bool arg1)
{
    if ( arg1 ) {
        ui->cmdGrp->show();
    } else {
        ui->cmdGrp->hide();
    }
}






void MainWindow::on_runCmdBtn_clicked()
{

}



void MainWindow::splitQString(const QString& text)
{
    m_splitQStringList.clear();
    auto sz = text.size();
    for( auto i = 0;  i < sz; ++i ) {
        QString singleCharacter = text.mid(i,1);
        auto ucs4Ary = singleCharacter.toUcs4();
        if ( !ucs4Ary.isEmpty() && QChar(ucs4Ary[0]) == QChar::ReplacementCharacter ) {
            // invalid character(s) , pick next Qchar
            if ( (i+1) < sz ) {
                // 2 Chars      ==> valid character 
                m_splitQStringList.push_back( WrapQString(i,2) );
                ++i;
            } // else { /* it is a lonely harf-character */ , it requires another half to combine a entire valid character.  }
        } else {
            // 1 Char ==> valid character 
            m_splitQStringList.push_back( WrapQString(i,1) );
        }

    }

}


void MainWindow::on_actionShow_Command_UserManual_triggered()
{
}

void MainWindow::disableHightLight()
{
    ui->targetScanTextBox->setExtraSelections( QList<QTextEdit::ExtraSelection>() );

}

void MainWindow::highLightAllMatched()
{
    QList<QTextEdit::ExtraSelection> lst;

    for ( auto i = 0; i < m_matchedPairList.size(); ++i ) {
        const auto& pr = m_matchedPairList[i];

        auto cursor = ui->targetScanTextBox->textCursor();
        QTextEdit::ExtraSelection selection;
        cursor.setPosition( pr.first, QTextCursor::MoveAnchor);
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  pr.second );
        selection.cursor = cursor;
        selection.format.setBackground( QBrush(Qt::yellow) );

        lst.push_back(selection);

    }
    ui->targetScanTextBox->setExtraSelections(lst);
}



/*


HighlightVec hvec;
hvec.push_back( qMakePair(qMakePair(begIdx,endIdx), Qt::red) );
hightLightNodeText(hvec);


void MainWindow::hightLightNodeText(const QVector< QPair<QPair<int,int>, QColor>>& hightLightInfoVec)
{
    QList<QTextEdit::ExtraSelection> lst;
    for ( const auto& info : hightLightInfoVec ) {
        auto begIdx = info.first.first;
        auto endIdx = info.first.second;
        auto highlightColor = info.second;

        auto cursor = ui->xmlTextEdit->textCursor();
        // QTextCursor currentCursor(cursor);
        
        QTextEdit::ExtraSelection selection;
        cursor.setPosition( begIdx, QTextCursor::MoveAnchor);
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  endIdx - begIdx );
        selection.cursor = cursor;
        selection.format.setBackground( QBrush(highlightColor) );

        lst.push_back(selection);
    }

    ui->xmlTextEdit->setExtraSelections(lst);
}

void MainWindow::setXMLCurrentTextCursor(int cursorIdx)
{
    QTextCursor currentCursor( ui->xmlTextEdit->textCursor() );
    auto realIdx = cursorIdx<=0 ? 0 : cursorIdx;
    currentCursor.setPosition(realIdx, QTextCursor::MoveAnchor);
    ui->xmlTextEdit->setTextCursor(currentCursor);
}
*/



void MainWindow::on_deHighlightBtn_clicked()
{
    disableHightLight();
}


void MainWindow::searchMatched(int startIdx , bool isOneMatchedStop)
{
    auto targetText2Scan = ui->targetScanTextBox->toPlainText();
    if ( targetText2Scan.isEmpty() ) {
        return;
    }

    if ( startIdx < 0 ) {
        startIdx = 0;
    }

    disableHightLight();
    m_matchedPairList.clear();
    auto contentSz = targetText2Scan.size();
    for ( auto wordIdx = startIdx; wordIdx < contentSz; ) {

        auto bFoundMatched = false;
        auto matchedLen = -1;
        for ( auto it = m_cfgObjectList.begin(); it != m_cfgObjectList.end(); ++it ) {
            if ( !it->enabled ) {
                continue;
            }

            for( auto repIt = it->variants_ch_Parttern.begin(); repIt!= it->variants_ch_Parttern.end(); ++ repIt  )
            {
                const QString& repStr = *repIt;
                auto repStrSize = repStr.size();
                if ( repStrSize > 0 ) {
                    auto partstr = targetText2Scan.mid(wordIdx, repStrSize);
                    if ( partstr == repStr ) {
                        m_matchedPairList.push_back( qMakePair(wordIdx, repStrSize) );
                        matchedLen = repStrSize;
                        bFoundMatched = true;
                        break; // break variants list
                    }
                }
             }

             if ( bFoundMatched ) {
                 break; // break iterator cfg table
             }
        }

        if ( bFoundMatched ) {
            if ( isOneMatchedStop ) {
                break;
            } else {
                wordIdx += matchedLen;
            }
        } else {
            ++wordIdx;
        }
    }

}


// void MainWindow::on_searchAllBtn_clicked()
// {
//     auto startIdx = 0;
//     auto bIsOneMatchedStop = false;
//     searchMatched(startIdx, bIsOneMatchedStop);
//     m_TextChangedStateAfterScan = 1;
// }


void MainWindow::highLightOneMatched(bool bShouldClearPreviousHighLight)
{
    if ( bShouldClearPreviousHighLight ) {
        disableHightLight();
    }

    auto matchedSz = m_matchedPairList.size();
    if ( matchedSz>0 && m_matchedIdx>=0 && m_matchedIdx < matchedSz ) {
        auto cursor = ui->targetScanTextBox->textCursor();
        const auto& matchedPair = m_matchedPairList[m_matchedIdx];

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // high light
        QList<QTextEdit::ExtraSelection> lst;
        QTextEdit::ExtraSelection selection;
        cursor.setPosition( matchedPair.first, QTextCursor::MoveAnchor);
        cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor,  matchedPair.second );
        selection.cursor = cursor;
        selection.format.setBackground( QBrush(Qt::yellow) );
        lst.push_back(selection);
        ui->targetScanTextBox->setExtraSelections(lst);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // move cursor back to the matched beginning position rather than match end position 
        cursor.setPosition( matchedPair.first, QTextCursor::MoveAnchor);
        ui->targetScanTextBox->setTextCursor(cursor);
        ui->targetScanTextBox->setFocus( Qt::ActiveWindowFocusReason );
    }
}


void MainWindow::on_targetScanTextBox_textChanged()
{
    if ( m_TextChangedStateAfterScan == 1 ) {
        m_TextChangedStateAfterScan = -1;
    }
}



void MainWindow::on_actionPreviousFind_triggered()
{
    if ( m_TextChangedStateAfterScan == -1 ) {
        qDebug() << "After scan , the text has been changed. Please scan it again.";
        return;
    }

    auto matchedSz = m_matchedPairList.size();
    if ( m_matchedIdx == -1 || matchedSz == 0 ) {
        // no matched , TODO
        return;
    }

    auto bIsJumpFlag = false;
    auto bIsLoopFlag = ui->loopChk->isChecked();
    if ( bIsLoopFlag ) {
        m_matchedIdx = (m_matchedIdx-1+matchedSz) % matchedSz;
        bIsJumpFlag = (matchedSz>1);
    } else {
        --m_matchedIdx;
        if ( m_matchedIdx < 0 ) {
            m_matchedIdx = 0;
        } else {
            bIsJumpFlag = true;
        }
    }

    // do highlight one item
    if ( bIsJumpFlag ) {
        qDebug() << "bIsJumpFlag , jump next : " << m_matchedIdx;
        highLightOneMatched(true);
    }



}

void MainWindow::on_actionNextFind_triggered()
{
    if ( m_TextChangedStateAfterScan == -1 ) {
        qDebug() << "After scan , the text has been changed. Please scan it again.";
        return;
    }

    auto matchedSz = m_matchedPairList.size();
    if ( m_matchedIdx == -1 || matchedSz == 0 ) {
        // no
        return;
    }

    auto bIsJumpFlag = false;
    auto bIsLoopFlag = ui->loopChk->isChecked();
    if ( bIsLoopFlag ) {
        m_matchedIdx = (m_matchedIdx+1+matchedSz) % matchedSz;
        bIsJumpFlag = (matchedSz>1);
    } else {
        ++m_matchedIdx;
        if ( m_matchedIdx >= matchedSz ) {
            m_matchedIdx = matchedSz-1;
        } else {
            bIsJumpFlag = true;
        }
    }

    // do highlight one item
    if ( bIsJumpFlag ) {
        qDebug() << "bIsJumpFlag , jump previous : " << m_matchedIdx;
        highLightOneMatched(true);
    }
}
