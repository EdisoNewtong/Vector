#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QPair>
#include <QSpinBox>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
public:
        struct settingInfo
        {
            //////////////////////////////////////////////////
            // +
            bool bIsAddGrpEnabled;
            int  iAddQuestionsCnt;

            int  iAdd1RangeMinRangeMin;
            int  iAdd1RangeMinRangeMax;
             int  iAdd1RangeMinUIValue;
            int  iAdd1RangeMaxRangeMin;
            int  iAdd1RangeMaxRangeMax;
             int  iAdd1RangeMaxUIValue;

            int  iAdd2RangeMinRangeMin;
            int  iAdd2RangeMinRangeMax;
              int  iAdd2RangeMinUIValue;
            int  iAdd2RangeMaxRangeMin;
            int  iAdd2RangeMaxRangeMax;
              int  iAdd2RangeMaxUIValue;

            int  iAdd1rate;
            int  iAdd2rate;
            int  iAdd3rate;

            //////////////////////////////////////////////////
            // -
            bool bIsMinusGrpEnabled;
            int  iMinusQuestionsCnt;
            bool bIsResultNegativeEnabled;

            int  iMinus1RangeMinRangeMin;
            int  iMinus1RangeMinRangeMax;
              int  iMinus1RangeMinUIValue;
            int  iMinus1RangeMaxRangeMin;
            int  iMinus1RangeMaxRangeMax;
              int  iMinus1RangeMaxUIValue;

            int  iMinus2RangeMinRangeMin;
            int  iMinus2RangeMinRangeMax;
              int  iMinus2RangeMinUIValue;
            int  iMinus2RangeMaxRangeMin;
            int  iMinus2RangeMaxRangeMax;
              int  iMinus2RangeMaxUIValue;

            int  iMinus1rate;
            int  iMinus2rate;
            int  iMinus3rate;

            //////////////////////////////////////////////////
            // *
            bool bIsMultiplyGrpEnabled;
            int  iMultiplyQuestionsCnt;

            int  iMultiply1RangeMinRangeMin;
            int  iMultiply1RangeMinRangeMax;
              int  iMultiply1RangeMinUIValue;
            int  iMultiply1RangeMaxRangeMin;
            int  iMultiply1RangeMaxRangeMax;
              int  iMultiply1RangeMaxUIValue;

            int  iMultiply2RangeMinRangeMin;
            int  iMultiply2RangeMinRangeMax;
              int  iMultiply2RangeMinUIValue;
            int  iMultiply2RangeMaxRangeMin;
            int  iMultiply2RangeMaxRangeMax;
              int  iMultiply2RangeMaxUIValue;

            int  iMultiply1rate;
            int  iMultiply2rate;
            int  iMultiply3rate;


            //////////////////////////////////////////////////
            // divide
            bool bIsDivideGrpEnabled;
            int  iDivideQuestionsCnt;
            bool bIs9x9;
            bool bIsResultHaveModEnabled;

            int  iDivide1RangeMinRangeMin;
            int  iDivide1RangeMinRangeMax;
              int  iDivide1RangeMinUIValue;
            int  iDivide1RangeMaxRangeMin;
            int  iDivide1RangeMaxRangeMax;
              int  iDivide1RangeMaxUIValue;

            int  iDivide2RangeMinRangeMin;
            int  iDivide2RangeMinRangeMax;
              int  iDivide2RangeMinUIValue;
            int  iDivide2RangeMaxRangeMin;
            int  iDivide2RangeMaxRangeMax;
              int  iDivide2RangeMaxUIValue;

            int  iDivide1rate;
            int  iDivide2rate;
            int  iDivide3rate;
        };
public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();



    void updateFocus();
    void reset_ui(const Dialog::settingInfo& cfg);
    const Dialog::settingInfo& getCurrentCfg();

private slots:
    void on_loadBtn_clicked();

    void on_saveBtn_clicked();

    void on_confirmBtn_clicked();

    void on_cancelBtn_clicked();


    // self code
    QPair<bool,QString> checkMinMaxValueRelationShip_Add1();
    QPair<bool,QString> checkMinMaxValueRelationShip_Add2();
    void adjustAddRate1();
    void adjustAddRate2();
    void adjustAddRate3();

    QPair<bool,QString> checkMinMaxValueRelationShip_Minus1();
    QPair<bool,QString> checkMinMaxValueRelationShip_Minus2();
    void adjustMinusRate1();
    void adjustMinusRate2();
    void adjustMinusRate3();

    QPair<bool,QString> checkMinMaxValueRelationShip_Multiply1();
    QPair<bool,QString> checkMinMaxValueRelationShip_Multiply2();
    void adjustMultiplyRate1();
    void adjustMultiplyRate2();
    void adjustMultiplyRate3();

    QPair<bool,QString> checkMinMaxValueRelationShip_Divide1();
    QPair<bool,QString> checkMinMaxValueRelationShip_Divide2();
    void adjustDivideRate1();
    void adjustDivideRate2();
    void adjustDivideRate3();

    void onErrorMsgTimeout();
    // void on_dummyBtn_clicked();

private:
    Ui::Dialog *ui;

    static settingInfo s_defaultSettings;
    settingInfo m_currentSettings;

    void registerEventHandler();
    void uiState2Cfg();

    QPair<bool,QString> minusSameLogic(int flag);
    QPair<bool,QString> divideSameLogic(int flag);

    void percentageAdjust(QSpinBox* current, QSpinBox* o1, QSpinBox* o2);

    QTimer errorTimer;
};

#endif // SETTINGDIALOG_H
