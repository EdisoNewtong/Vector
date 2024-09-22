#include "settingdialog.h"
#include "ui_dialog.h"
#include <QToolTip>

#include <QRandomGenerator>
#include <QBoxLayout>
#include <QMessageBox>

#include <QFileDialog> 
#include <QFile> 
#include <QTextStream>
#include <QMap>

#include <QDebug>
#include <QFont>


static const QString cstr_true("true");
static const QString cstr_false("false");



// static 
Dialog::settingInfo Dialog::s_defaultSettings{
    /*.bIsAddGrpEnabled = */ true,
     /*.iAddQuestionsCnt = */ 25,

     /*.iAdd1RangeMinRangeMin = */ 1,
     /*.iAdd1RangeMinRangeMax = */ 100,
       /*.iAdd1RangeMinUIValue = */  1,
     /*.iAdd1RangeMaxRangeMin = */ 1,
     /*.iAdd1RangeMaxRangeMax = */ 100,
       /*.iAdd1RangeMaxUIValue = */  100,

     /*.iAdd2RangeMinRangeMin = */ 1,
     /*.iAdd2RangeMinRangeMax = */ 100,
       /*.iAdd2RangeMinUIValue = */ 1,
     /*.iAdd2RangeMaxRangeMin = */ 1,
     /*.iAdd2RangeMaxRangeMax = */ 100,
       /*.iAdd2RangeMaxUIValue = */ 100,

     /*.iAdd1rate = */ 25,  // 25%
     /*.iAdd2rate = */ 25,  // 25%
     /*.iAdd3rate = */ 50,  // 50%

    //////////////////////////////////////////////////
    // -
    /*.bIsMinusGrpEnabled = */ true,
     /*.iMinusQuestionsCnt = */ 25,
     /*.bIsResultNegativeEnabled = */ false,

     /*.iMinus1RangeMinRangeMin = */ 1,
     /*.iMinus1RangeMinRangeMax = */ 100,
       /*.iMinus1RangeMinValue = */ 1,
     /*.iMinus1RangeMaxRangeMin = */ 1,
     /*.iMinus1RangeMaxRangeMax = */ 100,
       /*.iMinus1RangeMaxValue = */ 100,

     /*.iMinus2RangeMinRangeMin = */ 1,
     /*.iMinus2RangeMinRangeMax = */ 100,
       /*.iMinus2RangeMinValue = */ 1,
     /*.iMinus2RangeMaxRangeMin = */ 1,
     /*.iMinus2RangeMaxRangeMax = */ 100,
       /*.iMinus2RangeMaxValue = */ 100,

    /*.iMinus1rate = */ 25, // 25%
    /*.iMinus2rate = */ 25, // 25%
    /*.iMinus3rate = */ 50, // 50%

    //////////////////////////////////////////////////
    // *
    /*.bIsMultiplyGrpEnabled = */ true,
     /*.iMultiplyQuestionsCnt = */ 25,

     /*.iMultiply1RangeMinRangeMin = */ 1,
     /*.iMultiply1RangeMinRangeMax = */ 100,
       /*.iMultiply1RangeMinValue = */ 1,
     /*.iMultiply1RangeMaxRangeMin = */ 1,
     /*.iMultiply1RangeMaxRangeMax = */ 100,
       /*.iMultiply1RangeMaxValue = */ 100,

     /*.iMultiply2RangeMinRangeMin = */ 1,
     /*.iMultiply2RangeMinRangeMax = */ 100,
       /*.iMultiply2RangeMinValue = */ 1,
     /*.iMultiply2RangeMaxRangeMin = */ 1,
     /*.iMultiply2RangeMaxRangeMax = */ 100,
       /*.iMultiply2RangeMaxValue = */ 100,

     /*.iMultiply1rate = */ 25, // 25%
     /*.iMultiply2rate = */ 25, // 25%
     /*.iMultiply3rate = */ 50, // 50%


    //////////////////////////////////////////////////
    // divide
    /* .bIsDivideGrpEnabled = */ true,
    /* .iDivideQuestionsCnt = */ 25,
     /*.bIs9x9 = */ false,
     /*.bIsResultHaveModEnabled = */ false,

     /*.iDivide1RangeMinRangeMin = */ 1,
     /*.iDivide1RangeMinRangeMax = */ 100,
       /*.iDivide1RangeMinValue = */ 1,
     /*.iDivide1RangeMaxRangeMin = */ 1,
     /*.iDivide1RangeMaxRangeMax = */ 100,
       /*.iDivide1RangeMaxValue = */ 100,

     /*.iDivide2RangeMinRangeMin = */ 1,
     /*.iDivide2RangeMinRangeMax = */ 100,
       /*.iDivide2RangeMinValue = */ 1,
     /*.iDivide2RangeMaxRangeMin = */ 1,
     /*.iDivide2RangeMaxRangeMax = */ 100,
       /*.iDivide2RangeMaxValue = */ 100,

     /*.iDivide1rate = */ 25, // 25%
     /*.iDivide2rate = */ 25, // 25%
     /*.iDivide3rate = */ 50, // 50%

};


Dialog::Dialog(QWidget *parent) :
      QDialog(parent)
    , ui(new Ui::Dialog)
    , m_currentSettings( s_defaultSettings )
    , errorTimer(this)
{
    ui->setupUi(this);

    ui->errorMsg->hide();
    auto dlglayout = qobject_cast<QBoxLayout*>( this->layout() );
    if ( dlglayout != nullptr ) {
        // 0,9,1
        dlglayout->setStretchFactor(ui->errorMsg, 0);
        dlglayout->setStretchFactor(ui->settingsPart, 9);
        dlglayout->setStretchFactor(ui->btnsPart,  1);
    }

    // ui->dummyBtn->hide();
    reset_ui( m_currentSettings );

    registerEventHandler();
}

void Dialog::registerEventHandler()
{
    // Timer
    errorTimer.setSingleShot(true);
    errorTimer.setInterval(3000);
    connect( &errorTimer, SIGNAL(timeout()), this, SLOT(onErrorMsgTimeout()) );


    // + 
    connect( ui->addn1minSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Add1()) );
    connect( ui->addn1maxSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Add1()) );
    connect( ui->addn2minSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Add2()) );
    connect( ui->addn2maxSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Add2()) );

      // check sum is 100% and dynamic adjust
      connect( ui->add1rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustAddRate1()) );
      connect( ui->add2rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustAddRate2()) );
      connect( ui->add3rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustAddRate3()) );


    // -
    connect( ui->minus1minSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Minus1()) );
    connect( ui->minus1maxSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Minus1()) );
    connect( ui->minus2minSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Minus2()) );
    connect( ui->minus2maxSpn,  SIGNAL(editingFinished()), this, SLOT(checkMinMaxValueRelationShip_Minus2()) );

      connect( ui->minus1rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustMinusRate1()) );
      connect( ui->minus2rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustMinusRate2()) );
      connect( ui->minus3rateSpn,  SIGNAL(editingFinished()), this, SLOT(adjustMinusRate3()) );

    // *
    connect( ui->mt1minSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Multiply1() ) );
    connect( ui->mt1maxSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Multiply1() ) );
    connect( ui->mt2minSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Multiply2() ) );
    connect( ui->mt2maxSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Multiply2() ) );
      
       connect( ui->multiply1rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustMultiplyRate1()) );
       connect( ui->multiply2rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustMultiplyRate2()) );
       connect( ui->multiply3rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustMultiplyRate3()) );

    // divide  /
    connect( ui->divide1minSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Divide1() ) );
    connect( ui->divide1maxSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Divide1() ) );
    connect( ui->divide2minSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Divide2() ) );
    connect( ui->divide2maxSpn, SIGNAL(editingFinished()), this, SLOT( checkMinMaxValueRelationShip_Divide2() ) );

       connect( ui->divide1rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustDivideRate1()) );
       connect( ui->divide2rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustDivideRate2()) );
       connect( ui->divide3rateSpn, SIGNAL(editingFinished()), this, SLOT(adjustDivideRate3()) );
}



void Dialog::reset_ui(const Dialog::settingInfo& cfg)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // +
    ui->addGrp->setChecked( cfg.bIsAddGrpEnabled );
    ui->addQuestionCntSpn->setValue( cfg.iAddQuestionsCnt );

    ui->addn1minSpn->setRange( cfg.iAdd1RangeMinRangeMin, cfg.iAdd1RangeMinRangeMax ); // [min ~ max]
    ui->addn1minSpn->setValue( cfg.iAdd1RangeMinUIValue );
    ui->addn1maxSpn->setRange( cfg.iAdd1RangeMaxRangeMin, cfg.iAdd1RangeMaxRangeMax ); // [min ~ max]
    ui->addn1maxSpn->setValue( cfg.iAdd1RangeMaxUIValue );

    ui->addn2minSpn->setRange( cfg.iAdd2RangeMaxRangeMin, cfg.iAdd2RangeMinRangeMax ); // [min ~ max]
    ui->addn2minSpn->setValue( cfg.iAdd2RangeMinUIValue );
    ui->addn2maxSpn->setRange( cfg.iAdd2RangeMaxRangeMin, cfg.iAdd2RangeMaxRangeMax ); // [min ~ max]
    ui->addn2maxSpn->setValue( cfg.iAdd2RangeMaxUIValue );

    ui->add1rateSpn->setValue( cfg.iAdd1rate );
    ui->add2rateSpn->setValue( cfg.iAdd2rate );
    ui->add3rateSpn->setValue( cfg.iAdd3rate );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // -
    ui->minusGrp->setChecked( cfg.bIsMinusGrpEnabled );
    ui->minusQuestionCntSpn->setValue( cfg.iMinusQuestionsCnt );
    ui->minusNegativeChk->setCheckState( cfg.bIsResultNegativeEnabled ? Qt::Checked : Qt::Unchecked );

    ui->minus1minSpn->setRange( cfg.iMinus1RangeMinRangeMin, cfg.iMinus1RangeMinRangeMax ); // [min ~ max]
    ui->minus1minSpn->setValue( cfg.iMinus1RangeMinUIValue );
    ui->minus1maxSpn->setRange( cfg.iMinus1RangeMaxRangeMin, cfg.iMinus1RangeMaxRangeMax ); // [min ~ max]
    ui->minus1maxSpn->setValue( cfg.iMinus1RangeMaxUIValue );

    ui->minus2minSpn->setRange( cfg.iMinus2RangeMinRangeMin, cfg.iMinus2RangeMinRangeMax ); // [min ~ max]
    ui->minus2minSpn->setValue( cfg.iMinus2RangeMinUIValue );
    ui->minus2maxSpn->setRange( cfg.iMinus2RangeMaxRangeMin, cfg.iMinus2RangeMaxRangeMax ); // [min ~ max]
    ui->minus2maxSpn->setValue( cfg.iMinus2RangeMaxUIValue );

    ui->minus1rateSpn->setValue( cfg.iMinus1rate );
    ui->minus2rateSpn->setValue( cfg.iMinus2rate );
    ui->minus3rateSpn->setValue( cfg.iMinus3rate );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // *
    ui->multiplyGrp->setChecked( cfg.bIsMultiplyGrpEnabled );
    ui->multiplyQuestionCntSpn->setValue( cfg.iMultiplyQuestionsCnt );

    ui->mt1minSpn->setRange( cfg.iMultiply1RangeMinRangeMin, cfg.iMultiply1RangeMinRangeMax ); // [min ~ max]
    ui->mt1minSpn->setValue( cfg.iMultiply1RangeMinUIValue ); 
    ui->mt1maxSpn->setRange( cfg.iMultiply1RangeMaxRangeMin, cfg.iMultiply1RangeMaxRangeMax ); // [min ~ max]
    ui->mt1maxSpn->setValue( cfg.iMultiply1RangeMaxUIValue ); 

    ui->mt2minSpn->setRange( cfg.iMultiply2RangeMinRangeMin, cfg.iMultiply2RangeMinRangeMax ); // [min ~ max]
    ui->mt2minSpn->setValue( cfg.iMultiply2RangeMinUIValue ); 
    ui->mt2maxSpn->setRange( cfg.iMultiply2RangeMaxRangeMin, cfg.iMultiply2RangeMaxRangeMax ); // [min ~ max]
    ui->mt2maxSpn->setValue( cfg.iMultiply2RangeMaxUIValue ); 

    ui->multiply1rateSpn->setValue( cfg.iMultiply1rate );
    ui->multiply2rateSpn->setValue( cfg.iMultiply2rate );
    ui->multiply3rateSpn->setValue( cfg.iMultiply3rate );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // divide  /
    ui->divideGrp->setChecked( cfg.bIsDivideGrpEnabled );
    ui->divideQuestionCntSpn->setValue( cfg.iDivideQuestionsCnt );
    ui->chk9x9->setCheckState( cfg.bIs9x9 ? Qt::Checked : Qt::Unchecked );
    ui->divideModChk->setCheckState( cfg.bIsResultHaveModEnabled ? Qt::Checked : Qt::Unchecked );

    ui->divide1minSpn->setRange( cfg.iDivide1RangeMinRangeMin, cfg.iDivide1RangeMinRangeMax ); // [min ~ max]
    ui->divide1minSpn->setValue( cfg.iDivide1RangeMinUIValue ); 
    ui->divide1maxSpn->setRange( cfg.iDivide1RangeMaxRangeMin, cfg.iDivide1RangeMaxRangeMax ); // [min ~ max]
    ui->divide1maxSpn->setValue( cfg.iDivide1RangeMaxUIValue ); 

    ui->divide2minSpn->setRange( cfg.iDivide2RangeMinRangeMin, cfg.iDivide2RangeMinRangeMax ); // [min ~ max]
    ui->divide2minSpn->setValue( cfg.iDivide2RangeMinUIValue );
    ui->divide2maxSpn->setRange( cfg.iDivide2RangeMaxRangeMin, cfg.iDivide2RangeMaxRangeMax ); // [min ~ max]
    ui->divide2maxSpn->setValue( cfg.iDivide2RangeMaxUIValue );

    ui->divide1rateSpn->setValue( cfg.iDivide1rate );
    ui->divide2rateSpn->setValue( cfg.iDivide2rate );
    ui->divide3rateSpn->setValue( cfg.iDivide3rate );

    updateFocus();
}

void Dialog::updateFocus()
{
    ui->loadBtn->clearFocus();
    // ui->dummyBtn->setFocus( Qt::ActiveWindowFocusReason );
    ui->confirmBtn->setFocus( Qt::ActiveWindowFocusReason );
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_loadBtn_clicked()
{    
    // 5 -> Group box checked state
    // 4 -> QCheckBox checked state
    // 3 -> QSpinBox   set minimum Value
    // 2 -> QSpinBox   set maximum Value
    // 1 -> QSpinBox   set current value
    static const QMap<QString, QPair<QWidget*,int> > G_keywordsMap{
        { QString("bIsAddGrpEnabled"), qMakePair(ui->addGrp,5) },
        { QString("iAddQuestionsCnt"), qMakePair(ui->addQuestionCntSpn,1) },

        { QString("iAdd1RangeMinRangeMin"), qMakePair(ui->addn1minSpn,3) },
        { QString("iAdd1RangeMinRangeMax"), qMakePair(ui->addn1minSpn,2) },
        { QString("iAdd1RangeMinUIValue"),  qMakePair(ui->addn1minSpn,1) },
        { QString("iAdd1RangeMaxRangeMin"), qMakePair(ui->addn1maxSpn,3) },
        { QString("iAdd1RangeMaxRangeMax"), qMakePair(ui->addn1maxSpn,2) },
        { QString("iAdd1RangeMaxUIValue"),  qMakePair(ui->addn1maxSpn,1) },

        { QString("iAdd2RangeMinRangeMin"),  qMakePair(ui->addn2minSpn,3) },
        { QString("iAdd2RangeMinRangeMax"),  qMakePair(ui->addn2minSpn,2) },
        { QString("iAdd2RangeMinUIValue"),   qMakePair(ui->addn2minSpn,1) },
        { QString("iAdd2RangeMaxRangeMin"),  qMakePair(ui->addn2maxSpn,3) },
        { QString("iAdd2RangeMaxRangeMax"),  qMakePair(ui->addn2maxSpn,2) },
        { QString("iAdd2RangeMaxUIValue"),  qMakePair(ui->addn2maxSpn,1) },

        { QString("iAdd1rate"), qMakePair(ui->add1rateSpn,1) },
        { QString("iAdd2rate"), qMakePair(ui->add2rateSpn,1) },
        { QString("iAdd3rate"), qMakePair(ui->add3rateSpn,1) },

        //////////////////////////////////////////////////
        // -
        { QString("bIsMinusGrpEnabled"),  qMakePair(ui->minusGrp, 5) },
        { QString("iMinusQuestionsCnt"),  qMakePair(ui->minusQuestionCntSpn,1) },
        { QString("bIsResultNegativeEnabled"),  qMakePair(ui->minusNegativeChk,4) },

        { QString("iMinus1RangeMinRangeMin"),  qMakePair(ui->minus1minSpn,3) },
        { QString("iMinus1RangeMinRangeMax"),  qMakePair(ui->minus1minSpn,2) },
        { QString("iMinus1RangeMinUIValue"),  qMakePair(ui->minus1minSpn,1) },
        { QString("iMinus1RangeMaxRangeMin"), qMakePair(ui->minus1maxSpn,3) },
        { QString("iMinus1RangeMaxRangeMax"), qMakePair(ui->minus1maxSpn,2) },
        { QString("iMinus1RangeMaxUIValue"), qMakePair(ui->minus1maxSpn,1) },

        { QString("iMinus2RangeMinRangeMin"), qMakePair(ui->minus2minSpn,3) },
        { QString("iMinus2RangeMinRangeMax"), qMakePair(ui->minus2minSpn,2) },
        { QString("iMinus2RangeMinUIValue"),  qMakePair(ui->minus2minSpn,1) },
        { QString("iMinus2RangeMaxRangeMin"), qMakePair(ui->minus2maxSpn,3) },
        { QString("iMinus2RangeMaxRangeMax"), qMakePair(ui->minus2maxSpn,2) },
        { QString("iMinus2RangeMaxUIValue"), qMakePair(ui->minus2maxSpn,1)  },

        { QString("iMinus1rate"),  qMakePair(ui->minus1rateSpn, 1) },
        { QString("iMinus2rate"),  qMakePair(ui->minus2rateSpn, 1) },
        { QString("iMinus3rate"),  qMakePair(ui->minus3rateSpn, 1) },

        //////////////////////////////////////////////////
        // *
        { QString("bIsMultiplyGrpEnabled"), qMakePair(ui->multiplyGrp, 5) },
        { QString("iMultiplyQuestionsCnt"), qMakePair(ui->multiplyQuestionCntSpn,1) },

        { QString("iMultiply1RangeMinRangeMin"), qMakePair(ui->mt1minSpn,3) },
        { QString("iMultiply1RangeMinRangeMax"), qMakePair(ui->mt1minSpn,2) },
        { QString("iMultiply1RangeMinUIValue"),  qMakePair(ui->mt1minSpn,1) },
        { QString("iMultiply1RangeMaxRangeMin"), qMakePair(ui->mt1maxSpn,3) },
        { QString("iMultiply1RangeMaxRangeMax"), qMakePair(ui->mt1maxSpn,2) },
        { QString("iMultiply1RangeMaxUIValue"), qMakePair(ui->mt1maxSpn,1) },

        { QString("iMultiply2RangeMinRangeMin"), qMakePair(ui->mt2minSpn,3) },
        { QString("iMultiply2RangeMinRangeMax"), qMakePair(ui->mt2minSpn,2) },
        { QString("iMultiply2RangeMinUIValue"), qMakePair(ui->mt2minSpn,1) },
        { QString("iMultiply2RangeMaxRangeMin"), qMakePair(ui->mt2maxSpn,3) },
        { QString("iMultiply2RangeMaxRangeMax"), qMakePair(ui->mt2maxSpn,2) },
        { QString("iMultiply2RangeMaxUIValue"), qMakePair(ui->mt2maxSpn,1) },

        { QString("iMultiply1rate"), qMakePair(ui->multiply1rateSpn,1) },
        { QString("iMultiply2rate"), qMakePair(ui->multiply2rateSpn,1) },
        { QString("iMultiply3rate"), qMakePair(ui->multiply3rateSpn,1) },


        //////////////////////////////////////////////////
        // divide
        { QString("bIsDivideGrpEnabled"), qMakePair(ui->divideGrp, 5) },
        { QString("iDivideQuestionsCnt"), qMakePair(ui->divideQuestionCntSpn,1) },
        { QString("bIsDivide9x9"), qMakePair(ui->chk9x9,4) },
        { QString("bIsResultHaveModEnabled"), qMakePair(ui->divideModChk, 4) },

        { QString("iDivide1RangeMinRangeMin"), qMakePair(ui->divide1minSpn, 3)  },
        { QString("iDivide1RangeMinRangeMax"), qMakePair(ui->divide1minSpn, 2) },
        { QString("iDivide1RangeMinUIValue"), qMakePair(ui->divide1minSpn, 1) },
        { QString("iDivide1RangeMaxRangeMin"), qMakePair(ui->divide1maxSpn, 3) },
        { QString("iDivide1RangeMaxRangeMax"), qMakePair(ui->divide1maxSpn, 2) },
        { QString("iDivide1RangeMaxUIValue"), qMakePair(ui->divide1maxSpn, 1) },

        { QString("iDivide2RangeMinRangeMin"), qMakePair(ui->divide2minSpn, 3) },
        { QString("iDivide2RangeMinRangeMax"), qMakePair(ui->divide2minSpn, 2) },
        { QString("iDivide2RangeMinUIValue"), qMakePair(ui->divide2minSpn, 1) },
        { QString("iDivide2RangeMaxRangeMin"), qMakePair(ui->divide2maxSpn, 3) },
        { QString("iDivide2RangeMaxRangeMax"), qMakePair(ui->divide2maxSpn, 2) },
        { QString("iDivide2RangeMaxUIValue"), qMakePair(ui->divide2maxSpn, 1) },

        { QString("iDivide1rate"), qMakePair(ui->divide1rateSpn, 1) },
        { QString("iDivide2rate"), qMakePair(ui->divide2rateSpn, 1) },
        { QString("iDivide3rate"), qMakePair(ui->divide3rateSpn, 1) }
    };


    auto fileToLoaded = QFileDialog::getOpenFileName( this, QStringLiteral("请选择一个配置文件 "), QString(), QString("4 operators Config files (*.cfg);;") );
    if ( fileToLoaded.isEmpty() ) {
        QMessageBox::information(this, "错误", "你取消了加载配置文件");
        return;
    }

    QFile loadedfile(fileToLoaded);
    auto openOK = loadedfile.open( QIODevice::ReadOnly );
    if ( !openOK ) {
        QMessageBox::warning(this, "错误", "无法打开配置文件");
        return;
    }

    QString fileContent = loadedfile.readAll();
    QStringList lines = fileContent.split( QChar('\n') );

    auto lineCnt = lines.size();
    for( auto i = 0; i < lineCnt; ++i )
    {
        QString oneLine = lines[i].trimmed();
        if ( !oneLine.startsWith( QString("//") ) ) {
            auto partKeyValue = oneLine.split( QChar('='), QString::SkipEmptyParts );
            if ( partKeyValue.size() == 2 ) {
                QString key = partKeyValue[0].trimmed();
                QString str_Value = partKeyValue[1].trimmed();
                auto it = G_keywordsMap.find( key );
                if ( it!=G_keywordsMap.end() ) {
                    QWidget* widget = it.value().first;
                    int  widgetType = it.value().second;

                    if ( widgetType == 5 ) {
                        QGroupBox* grpBox = qobject_cast<QGroupBox*>( widget );
                        if ( grpBox!=nullptr ) {
                            grpBox->setChecked( str_Value==cstr_true );
                        } else {
                            QMessageBox::warning(this, "错误", "group box is nullptr");
                        }
                    } else if ( widgetType == 4 ) {
                        QCheckBox* chkbox = qobject_cast<QCheckBox*>( widget );
                        if ( chkbox!=nullptr ) {
                            chkbox->setChecked( str_Value==cstr_true );
                        } else {
                            QMessageBox::warning(this, "错误", "check box is nullptr");
                        }
                    } else if ( widgetType == 3 || widgetType == 2 || widgetType == 1) {
                        QSpinBox* spinbox = qobject_cast<QSpinBox*>( widget );
                        if ( spinbox!=nullptr ) {


                            if ( widgetType == 3) {
                                spinbox->setMinimum( str_Value.toInt() );
                            } else if ( widgetType == 2 ) {
                                spinbox->setMaximum( str_Value.toInt() );
                            } else {
                                // set current value
                                spinbox->setValue( str_Value.toInt() );
                            }
                        } else {
                            QMessageBox::warning(this, "错误", "spinBox is nullptr");
                        }

                    } 
                }
            }
        }
    }

    QMessageBox::information(this, "文件加载", "成功读取配置文件");


}

void Dialog::on_saveBtn_clicked()
{
    QString exePath = qGuiApp->applicationDirPath();
    auto saveFileName = QFileDialog::getSaveFileName( this, "保存[四则运算]的配置文件", exePath, "4 operators Config files (*.cfg);;" );
    if ( saveFileName.isEmpty() ) {
        QMessageBox::information(this, "保存[四则运算]的配置 文件", "文件名不能为空");
        return;
    }


    QString filled;
    QString fileContent;

    // Header
    fileContent += QString("//////////////////////////////////////////////////\n// 4 Operator Config File \n");

    //////////////////////////////////////////////////
    // +
    fileContent += QString("//////////////////////////////////////////////////\n// + Plus\n");
    fileContent += QString("\t bIsAddGrpEnabled = %1\n").arg( ui->addGrp->isChecked() ? cstr_true : cstr_false );
    fileContent += QString("\t iAddQuestionsCnt  = %1\n").arg( ui->addQuestionCntSpn->value() );

    fileContent += QString("\t iAdd1RangeMinRangeMin = %1\n").arg( ui->addn1minSpn->minimum() );
    fileContent += QString("\t iAdd1RangeMinRangeMax = %1\n").arg( ui->addn1minSpn->maximum() );
     fileContent += QString("\t iAdd1RangeMinUIValue = %1\n").arg( ui->addn1minSpn->value() );
    fileContent += QString("\t iAdd1RangeMaxRangeMin = %1\n").arg( ui->addn1maxSpn->minimum() );
    fileContent += QString("\t iAdd1RangeMaxRangeMax = %1\n").arg( ui->addn1maxSpn->maximum() );
     fileContent += QString("\t iAdd1RangeMaxUIValue = %1\n").arg( ui->addn1maxSpn->value() );

    fileContent += QString("\t iAdd2RangeMinRangeMin = %1\n").arg( ui->addn2minSpn->minimum() );
    fileContent += QString("\t iAdd2RangeMinRangeMax = %1\n").arg( ui->addn2minSpn->maximum() );
      fileContent += QString("\t iAdd2RangeMinUIValue = %1\n").arg( ui->addn2minSpn->value() );
    fileContent += QString("\t iAdd2RangeMaxRangeMin = %1\n").arg( ui->addn2maxSpn->minimum() );
    fileContent += QString("\t iAdd2RangeMaxRangeMax = %1\n").arg( ui->addn2maxSpn->maximum() );
      fileContent += QString("\t iAdd2RangeMaxUIValue = %1\n").arg( ui->addn2maxSpn->value() );

    fileContent += QString("\t iAdd1rate = %1\n").arg( ui->add1rateSpn->value() );
    fileContent += QString("\t iAdd2rate = %1\n").arg( ui->add2rateSpn->value()  );
    fileContent += QString("\t iAdd3rate = %1\n\n\n").arg( ui->add3rateSpn->value() );

    //////////////////////////////////////////////////
    // -
    fileContent += QString("//////////////////////////////////////////////////\n// - Minus\n");
    fileContent += QString("\t bIsMinusGrpEnabled = %1\n").arg( ui->minusGrp->isChecked() ? cstr_true : cstr_false );
    fileContent += QString("\t  iMinusQuestionsCnt = %1\n").arg( ui->minusQuestionCntSpn->value() );
    fileContent += QString("\t bIsResultNegativeEnabled = %1\n").arg( ui->minusNegativeChk->isChecked() ? cstr_true : cstr_false );

    fileContent += QString("\t  iMinus1RangeMinRangeMin = %1\n").arg( ui->minus1minSpn->minimum() );
    fileContent += QString("\t  iMinus1RangeMinRangeMax = %1\n").arg( ui->minus1minSpn->maximum() );
      fileContent += QString("\t  iMinus1RangeMinUIValue = %1\n").arg( ui->minus1minSpn->value()  );
    fileContent += QString("\t  iMinus1RangeMaxRangeMin = %1\n").arg( ui->minus1maxSpn->minimum() );
    fileContent += QString("\t  iMinus1RangeMaxRangeMax = %1\n").arg( ui->minus1maxSpn->maximum() );
      fileContent += QString("\t  iMinus1RangeMaxUIValue = %1\n").arg( ui->minus1maxSpn->value()  );

    fileContent += QString("\t  iMinus2RangeMinRangeMin = %1\n").arg( ui->minus2minSpn->minimum() );
    fileContent += QString("\t  iMinus2RangeMinRangeMax = %1\n").arg( ui->minus2minSpn->maximum() );
      fileContent += QString("\t  iMinus2RangeMinUIValue = %1\n").arg( ui->minus2minSpn->value() );
    fileContent += QString("\t  iMinus2RangeMaxRangeMin = %1\n").arg( ui->minus2maxSpn->minimum() );
    fileContent += QString("\t  iMinus2RangeMaxRangeMax = %1\n").arg( ui->minus2maxSpn->maximum() );
      fileContent += QString("\t  iMinus2RangeMaxUIValue = %1\n").arg( ui->minus2maxSpn->value() );

    fileContent += QString("\t  iMinus1rate = %1\n").arg( ui->minus1rateSpn->value() );
    fileContent += QString("\t  iMinus2rate = %1\n").arg(  ui->minus2rateSpn->value() );
    fileContent += QString("\t  iMinus3rate = %1\n\n\n").arg(  ui->minus3rateSpn->value() );

    //////////////////////////////////////////////////
    // *
    fileContent += QString("//////////////////////////////////////////////////\n// ×  Multiply\n");
    fileContent += QString("\t bIsMultiplyGrpEnabled = %1\n").arg( ui->multiplyGrp->isChecked() ? cstr_true : cstr_false);
    fileContent += QString("\t  iMultiplyQuestionsCnt = %1\n").arg( ui->multiplyQuestionCntSpn->value() );

    fileContent += QString("\t  iMultiply1RangeMinRangeMin = %1\n").arg( ui->mt1minSpn->minimum() );
    fileContent += QString("\t  iMultiply1RangeMinRangeMax = %1\n").arg(  ui->mt1minSpn->maximum() );
      fileContent += QString("\t  iMultiply1RangeMinUIValue = %1\n").arg(  ui->mt1minSpn->value() );
    fileContent += QString("\t  iMultiply1RangeMaxRangeMin = %1\n").arg(  ui->mt1maxSpn->minimum() );
    fileContent += QString("\t  iMultiply1RangeMaxRangeMax = %1\n").arg(  ui->mt1maxSpn->maximum() );
      fileContent += QString("\t  iMultiply1RangeMaxUIValue = %1\n").arg(  ui->mt1maxSpn->value() );

    fileContent += QString("\t  iMultiply2RangeMinRangeMin = %1\n").arg(  ui->mt1minSpn->minimum() );
    fileContent += QString("\t  iMultiply2RangeMinRangeMax = %1\n").arg(  ui->mt1minSpn->maximum() );
      fileContent += QString("\t  iMultiply2RangeMinUIValue = %1\n").arg(  ui->mt1minSpn->value() );
    fileContent += QString("\t  iMultiply2RangeMaxRangeMin = %1\n").arg(  ui->mt2maxSpn->minimum() );
    fileContent += QString("\t  iMultiply2RangeMaxRangeMax = %1\n").arg(  ui->mt2maxSpn->maximum() );
      fileContent += QString("\t  iMultiply2RangeMaxUIValue = %1\n").arg(  ui->mt2maxSpn->value() );

    fileContent += QString("\t  iMultiply1rate = %1\n").arg( ui->multiply1rateSpn->value() );
    fileContent += QString("\t  iMultiply2rate = %1\n").arg( ui->multiply2rateSpn->value() );
    fileContent += QString("\t  iMultiply3rate = %1\n\n\n").arg( ui->multiply3rateSpn->value() );



    //////////////////////////////////////////////////
    // divide
    fileContent += QString("//////////////////////////////////////////////////\n// ÷   Divide\n");
    fileContent += QString("\t bIsDivideGrpEnabled = %1\n").arg( ui->divideGrp->isChecked() ? cstr_true : cstr_false);
    fileContent += QString("\t  iDivideQuestionsCnt = %1\n").arg( ui->divideQuestionCntSpn->value() );
    fileContent += QString("\t  bIsDivide9x9 = %1\n").arg( ui->chk9x9->isChecked() ?  cstr_true : cstr_false );
    fileContent += QString("\t bIsResultHaveModEnabled = %1\n").arg( ui->divideModChk->isChecked() ?  cstr_true : cstr_false );

    fileContent += QString("\t  iDivide1RangeMinRangeMin = %1\n").arg( ui->divide1minSpn->minimum() );
    fileContent += QString("\t  iDivide1RangeMinRangeMax = %1\n").arg( ui->divide1minSpn->maximum() );
      fileContent += QString("\t  iDivide1RangeMinUIValue = %1\n").arg( ui->divide1minSpn->value() );
    fileContent += QString("\t  iDivide1RangeMaxRangeMin = %1\n").arg( ui->divide1maxSpn->minimum() );
    fileContent += QString("\t  iDivide1RangeMaxRangeMax = %1\n").arg( ui->divide1maxSpn->maximum() );
      fileContent += QString("\t  iDivide1RangeMaxUIValue = %1\n").arg( ui->divide1maxSpn->value() );

    fileContent += QString("\t  iDivide2RangeMinRangeMin = %1\n").arg( ui->divide2minSpn->minimum() );
    fileContent += QString("\t  iDivide2RangeMinRangeMax = %1\n").arg( ui->divide2minSpn->maximum() );
      fileContent += QString("\t  iDivide2RangeMinUIValue = %1\n").arg( ui->divide2minSpn->value() );
    fileContent += QString("\t  iDivide2RangeMaxRangeMin = %1\n").arg( ui->divide2maxSpn->minimum() );
    fileContent += QString("\t  iDivide2RangeMaxRangeMax = %1\n").arg( ui->divide2maxSpn->maximum() );
      fileContent += QString("\t  iDivide2RangeMaxUIValue = %1\n").arg( ui->divide2maxSpn->value() );

    fileContent += QString("\t  iDivide1rate = %1\n").arg( ui->divide1rateSpn->value() );
    fileContent += QString("\t  iDivide2rate = %1\n").arg( ui->divide2rateSpn->value() );
    fileContent += QString("\t  iDivide3rate = %1\n\n\n").arg( ui->divide3rateSpn->value() );

    QFile file( saveFileName );
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QMessageBox::information(this, "保存[四则运算]的配置文件 失败", "保存失败");
        return;
    }

    QTextStream outstream(&file);
    outstream << fileContent;
    file.flush();
    file.close();

    QMessageBox::information(this, "保存文件", "保存成功");
}

void Dialog::on_confirmBtn_clicked()
{

    if (     checkMinMaxValueRelationShip_Add1().first
          && checkMinMaxValueRelationShip_Add2().first
          && checkMinMaxValueRelationShip_Minus1().first
          && checkMinMaxValueRelationShip_Minus2().first
          && checkMinMaxValueRelationShip_Multiply1().first
          && checkMinMaxValueRelationShip_Multiply2().first
          && checkMinMaxValueRelationShip_Divide1().first
          && checkMinMaxValueRelationShip_Divide2().first 
    ) 
    {
        // store it into varible
        {
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // +   isEnabled();   ->value()  isChecked()
            m_currentSettings.bIsAddGrpEnabled = ui->addGrp->isChecked();
            m_currentSettings.iAddQuestionsCnt = ui->addQuestionCntSpn->value();

            m_currentSettings.iAdd1RangeMinUIValue = ui->addn1minSpn->value();
            m_currentSettings.iAdd1RangeMaxUIValue = ui->addn1maxSpn->value();
            m_currentSettings.iAdd2RangeMinUIValue = ui->addn2minSpn->value();
            m_currentSettings.iAdd2RangeMaxUIValue = ui->addn2maxSpn->value();

            m_currentSettings.iAdd1rate = ui->add1rateSpn->value();
            m_currentSettings.iAdd2rate = ui->add2rateSpn->value();
            m_currentSettings.iAdd3rate = ui->add3rateSpn->value();

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // -
            m_currentSettings.bIsMinusGrpEnabled = ui->minusGrp->isChecked();
            m_currentSettings.iMinusQuestionsCnt = ui->minusQuestionCntSpn->value();
            m_currentSettings.bIsResultNegativeEnabled = ui->minusNegativeChk->isChecked();

            m_currentSettings.iMinus1RangeMinUIValue =  ui->minus1minSpn->value();
            m_currentSettings.iMinus1RangeMaxUIValue =  ui->minus1maxSpn->value();

            m_currentSettings.iMinus2RangeMinUIValue = ui->minus2minSpn->value();
            m_currentSettings.iMinus2RangeMaxUIValue = ui->minus2maxSpn->value();

            m_currentSettings.iMinus1rate = ui->minus1rateSpn->value();
            m_currentSettings.iMinus2rate = ui->minus2rateSpn->value();
            m_currentSettings.iMinus3rate = ui->minus3rateSpn->value();

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // *
            m_currentSettings.bIsMultiplyGrpEnabled = ui->multiplyGrp->isChecked();
            m_currentSettings.iMultiplyQuestionsCnt = ui->multiplyQuestionCntSpn->value();

            m_currentSettings.iMultiply1RangeMinUIValue = ui->mt1minSpn->value();
            m_currentSettings.iMultiply1RangeMaxUIValue = ui->mt1maxSpn->value();

            m_currentSettings.iMultiply2RangeMinUIValue = ui->mt2minSpn->value();
            m_currentSettings.iMultiply2RangeMaxUIValue = ui->mt2maxSpn->value();

            m_currentSettings.iMultiply1rate = ui->multiply1rateSpn->value();
            m_currentSettings.iMultiply2rate = ui->multiply2rateSpn->value();
            m_currentSettings.iMultiply3rate = ui->multiply3rateSpn->value();

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // divide  /
            m_currentSettings.bIsDivideGrpEnabled = ui->divideGrp->isChecked();
            m_currentSettings.iDivideQuestionsCnt = ui->divideQuestionCntSpn->value();
            m_currentSettings.bIs9x9 = ui->chk9x9->isChecked();
            m_currentSettings.bIsResultHaveModEnabled = ui->divideModChk->isChecked();

            m_currentSettings.iDivide1RangeMinUIValue = ui->divide1minSpn->value();
            m_currentSettings.iDivide1RangeMaxUIValue = ui->divide1maxSpn->value();

            m_currentSettings.iDivide2RangeMinUIValue = ui->divide2minSpn->value();
            m_currentSettings.iDivide2RangeMaxUIValue = ui->divide2maxSpn->value();

            m_currentSettings.iDivide1rate = ui->divide1rateSpn->value();
            m_currentSettings.iDivide2rate = ui->divide2rateSpn->value();
            m_currentSettings.iDivide3rate = ui->divide3rateSpn->value();
        }

        this->accept();
    }
    else
    {
        QMessageBox::warning(this, QString("设置表中有错误"), QString("具体请看窗口顶部红色的错误内容") );
        return;
    }
}

void Dialog::on_cancelBtn_clicked()
{
    this->reject();
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Add1()
{
    auto minValue = ui->addn1minSpn->value();
    auto maxValue = ui->addn1maxSpn->value();
    if ( minValue > maxValue ) {
        QString msg("加数#1的最小值 [不能]大于 加数#1的最大值");
        ui->errorMsg->setText(msg);
        ui->errorMsg->show();
        errorTimer.start(); 
        return qMakePair(false, msg);
    }

    return qMakePair(true,QString());
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Add2()
{
    auto minValue = ui->addn2minSpn->value();
    auto maxValue = ui->addn2maxSpn->value();
    if ( minValue > maxValue ) {
        QString msg("加数#2的最小值 [不能]大于 加数#2的最大值");
        ui->errorMsg->setText(msg);
        ui->errorMsg->show();
        errorTimer.start(); 
        return qMakePair(false, msg);
    }

    return qMakePair(true,QString());
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Minus1()
{
    return minusSameLogic(1);
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Minus2()
{
    return minusSameLogic(2);
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Multiply1()
{
    auto minValue = ui->mt1minSpn->value();
    auto maxValue = ui->mt1maxSpn->value();
    if ( minValue > maxValue ) {
        QString msg("乘数#1的最小值 [不能]大于 乘数#1的最大值");
        ui->errorMsg->setText(msg);
        ui->errorMsg->show();
        errorTimer.start(); 
        return qMakePair(false, msg);
    }

    return qMakePair(true,QString());
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Multiply2()
{
    auto minValue = ui->mt2minSpn->value();
    auto maxValue = ui->mt2maxSpn->value();
    if ( minValue > maxValue ) {
        QString msg("乘数#2的最小值 [不能]大于 乘数#2的最大值");
        ui->errorMsg->setText(msg);
        ui->errorMsg->show();
        errorTimer.start();
        return qMakePair(false, msg);
    }

    return qMakePair(true,QString());
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Divide1()
{
    return divideSameLogic(1);
}

QPair<bool,QString> Dialog::checkMinMaxValueRelationShip_Divide2()
{
    return divideSameLogic(2);
}


QPair<bool,QString> Dialog::minusSameLogic(int flag)
{
    auto canBeNegative = ui->minusNegativeChk->isChecked();

    auto minValue_1 = ui->minus1minSpn->value();
    auto maxValue_1 = ui->minus1maxSpn->value();
    auto minValue_2 = ui->minus2minSpn->value();
    auto maxValue_2 = ui->minus2maxSpn->value();

    if ( flag == 1 ) {
        if ( minValue_1 > maxValue_1 ) {
            QString msg("被减数的最小值 [不能]大于 被减数的最大值");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    } else if ( flag == 2 ) {
        if ( minValue_2 > maxValue_2 ) {
            QString msg("减数的最小值 [不能]大于 减数的最大值");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    }

    if ( !canBeNegative ) {
        if ( maxValue_1 < minValue_2 ) {
            QString msg("被减数的最大值 [不能]小于 减数的最小值，不然 差数为负数");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    }

    return qMakePair(true,QString());
}


QPair<bool,QString> Dialog::divideSameLogic(int flag)
{
    auto canBeMod = ui->divideModChk->isChecked();

    auto minValue_1 = ui->divide1minSpn->value();
    auto maxValue_1 = ui->divide1maxSpn->value();
    auto minValue_2 = ui->divide2minSpn->value();
    auto maxValue_2 = ui->divide2maxSpn->value();

    if ( flag == 1 ) {
        if ( minValue_1 > maxValue_1 ) {
            QString msg("被除数的最小值 [不能]大于 被除数的最大值");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    } else if ( flag == 2 ) {
        if ( minValue_2 > maxValue_2 ) {
            QString msg("除数的最小值 [不能]大于 除数的最大值");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    }

    if ( !canBeMod ) {
        if ( maxValue_1 < minValue_2 ) {
            QString msg("被除数的的范围 [不能]小于 除数的范围，不然 商会变成 分数");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }

        auto allFailed = true;
        for( auto a = minValue_1; a <= maxValue_1; ++a ) {
            for( auto b = minValue_2; b <= maxValue_2; ++b ) {
                if ( a % b == 0 ) {
                    allFailed = false;
                    break;
                }
            }

            if ( !allFailed ) {
                break;
            }
        }

        if ( allFailed ) {
            QString msg("被除数的的范围 / 除数的范围，都无法 整除");
            ui->errorMsg->setText(msg);
            ui->errorMsg->show();
            errorTimer.start(); 
            return qMakePair(false, msg);
        }
    }

    return qMakePair(true,QString());
}



void Dialog::adjustAddRate1() { percentageAdjust(ui->add1rateSpn, ui->add2rateSpn, ui->add3rateSpn); }
void Dialog::adjustAddRate2() { percentageAdjust(ui->add2rateSpn, ui->add1rateSpn, ui->add3rateSpn); }
void Dialog::adjustAddRate3() { percentageAdjust(ui->add3rateSpn, ui->add1rateSpn, ui->add2rateSpn); }

void Dialog::adjustMinusRate1() { percentageAdjust(ui->minus1rateSpn, ui->minus2rateSpn,ui->minus3rateSpn );  }
void Dialog::adjustMinusRate2() { percentageAdjust(ui->minus2rateSpn, ui->minus1rateSpn,ui->minus3rateSpn );  }
void Dialog::adjustMinusRate3() { percentageAdjust(ui->minus3rateSpn, ui->minus1rateSpn,ui->minus2rateSpn );  }


void Dialog::adjustMultiplyRate1() { percentageAdjust(ui->multiply1rateSpn, ui->multiply2rateSpn, ui->multiply3rateSpn ); }
void Dialog::adjustMultiplyRate2() { percentageAdjust(ui->multiply2rateSpn, ui->multiply1rateSpn, ui->multiply3rateSpn ); }
void Dialog::adjustMultiplyRate3() { percentageAdjust(ui->multiply3rateSpn, ui->multiply1rateSpn, ui->multiply2rateSpn ); }

void Dialog::adjustDivideRate1() { percentageAdjust(ui->divide1rateSpn, ui->divide2rateSpn, ui->divide3rateSpn); }
void Dialog::adjustDivideRate2() { percentageAdjust(ui->divide2rateSpn, ui->divide1rateSpn, ui->divide3rateSpn); }
void Dialog::adjustDivideRate3() { percentageAdjust(ui->divide3rateSpn, ui->divide1rateSpn, ui->divide2rateSpn); }



void Dialog::percentageAdjust(QSpinBox* current, QSpinBox* o1, QSpinBox* o2)
{
    auto c = 0;
    if ( current!=nullptr ) {
        c = current->value();
    }

    auto another1 = o1;
    auto another2 = o2;

    auto p1 = 0;
    if ( another1!=nullptr ) {
        p1 = another1->value();
    }

    auto p2 = 0;
    if ( another2!=nullptr ) {
        p2 = another2->value();
    }

    auto total = c+p1+p2;
    if ( total!=100 ) {
        int ranNumber = static_cast<int>( QRandomGenerator::global()->generate() );
        QSpinBox* picked = nullptr;
        QSpinBox* other = nullptr;
        auto pickedValue = 0;
        auto otherValue = 0;

        // ranNumber is an odd number
        if ( (ranNumber & 1) == 1 ) {
            picked = another1; pickedValue = p1;
            other = another2;  otherValue  = p2;
        } else {
            picked = another2; pickedValue = p2;
            other = another1;  otherValue  = p1;
        }

        auto rest = 100 - c;
        auto half = rest / 2;
        auto canHalf = (rest % 2 == 0);

        auto p1Exceeded = (rest - p1) < 0;
        auto p2Exceeded = (rest - p2) < 0;

        // 25% 25% 50%
        //         50% -> 90%  ==> rest = 10%
        //  Both 25% are exceeded
        if ( p1Exceeded && p2Exceeded ) {
            // seperate both as half
            if ( canHalf ) {
                if ( picked!=nullptr ) {
                    picked->setValue( half );
                }

                if ( other!=nullptr ) {
                    other->setValue( half );
                }
            } else {
                if ( picked!=nullptr ) {
                    picked->setValue( half + 1 );
                }

                if ( other!=nullptr ) {
                    other->setValue( half );
                }
            }
        } else if ( p1Exceeded ) {
            auto newp1 = (rest - p2);
            if ( newp1 < 0 ) {
                newp1 = 0;
            }

            if ( another1!=nullptr ) {
                another1->setValue( newp1 );
            }
        } else if ( p2Exceeded ) {
            auto newp2 = (rest - p1);
            if ( newp2 < 0 ) {
                newp2 = 0;
            }

            if ( another2!=nullptr ) {
                another2->setValue( newp2 );
            }
        } else {
            // increase Both or   keep one and increase other one
            
            // keep p2 , change p1
            auto afterChanged = rest - otherValue;
            if ( picked != nullptr ) {
                picked->setValue( afterChanged );
            }

            Q_UNUSED(pickedValue)

        }

    }
}


void Dialog::onErrorMsgTimeout()
{
    ui->errorMsg->hide();
}



const Dialog::settingInfo& Dialog::getCurrentCfg()
{
    return m_currentSettings;
}






// void Dialog::on_dummyBtn_clicked()
// {
//    return;
// }



