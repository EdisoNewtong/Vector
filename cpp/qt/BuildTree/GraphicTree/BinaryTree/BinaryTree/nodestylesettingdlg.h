#ifndef NODESTYLESETTINGDLG_H
#define NODESTYLESETTINGDLG_H

#include <QDialog>
#include <QAbstractButton>
#include "treenode.h"
#include "nodestylecfg.h"


namespace Ui {
class NodeStyleSettingDlg;
}

class NodeStyleSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NodeStyleSettingDlg(QWidget *parent = nullptr);
    ~NodeStyleSettingDlg();

    void init(treenode* node);

protected:
    treenode* m_nodeToSet;

    nodeStyleCfg m_originalCfg;
    nodeStyleCfg m_toApplyCfg;

	bool m_bNeedUpdateAtOnce;

protected:
	void updateAllRenderObject();
private slots:
    void on_circleBrushSettingBtn_clicked();

    void on_circlePenSettingBtn_clicked();

    void on_textFontSettingBtn_clicked();

    void on_connectionLineSettingBtn_clicked();

    void on_textBrushSettingBtn_clicked();

    void on_circlePenWidthSetting_valueChanged(double arg1);

    void on_connectionLineWidthSetting_valueChanged(double arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_NodeStyleSettingDlg_finished(int result);

private:
    Ui::NodeStyleSettingDlg *ui;
};

#endif // NODESTYLESETTINGDLG_H