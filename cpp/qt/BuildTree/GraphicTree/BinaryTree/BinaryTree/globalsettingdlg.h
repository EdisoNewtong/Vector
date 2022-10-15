#ifndef GLOBALSETTINGDLG_H
#define GLOBALSETTINGDLG_H

#include <QDialog>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QLineEdit>

namespace Ui {
class GlobalSettingDlg;
}

class GlobalSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalSettingDlg(QWidget *parent, QBrush* pBg);
    ~GlobalSettingDlg();

private slots:
    void on_circleBrushSettingBtn_clicked();

    void on_circlePenSettingBtn_clicked();

    void on_textFontSettingBtn_clicked();

    void on_textBrushSettingBtn_clicked();

    void on_connectionLineSettingBtn_clicked();

    void on_resetDefaultBtn_clicked();

    void on_BgColorBtn_clicked();

    void on_circlePenWidthSetting_valueChanged(double arg1);

    void on_connectionLineWidthSetting_valueChanged(double arg1);

    void on_GlobalSettingDlg_finished(int result);

private:
    Ui::GlobalSettingDlg *ui;

	QBrush* m_pOutBg;
	QBrush m_sceneBg;

	QBrush m_circleBrush;
	QPen   m_circlePen;
	QFont  m_textFont;
	QBrush m_textColor;
	QPen   m_connectionLine;

	qreal  m_circleRadius;
	qreal  m_gap1;
	qreal  m_gap2;
	qreal  m_disHeight;

	qreal  m_leftMargin;
	qreal  m_rightMargin;
	qreal  m_topMargin;
	qreal  m_bottomMargin;

protected:
	void updateUI();
	bool updateValueFromEditor(QLineEdit* lineEdit, int tag, QString& errorMsg);

};

#endif // GLOBALSETTINGDLG_H
