#ifndef NODESTYLESETTINGDLG_H
#define NODESTYLESETTINGDLG_H

#include <QDialog>
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

	void init(const nodeStyleCfg& cfg);
private slots:
    void on_circleBrushSettingBtn_clicked();

    void on_circlePenSettingBtn_clicked();

    void on_textFontSettingBtn_clicked();

    void on_connectionLineSettingBtn_clicked();

    void on_textBrushSettingBtn_clicked();

private:
    Ui::NodeStyleSettingDlg *ui;
};

#endif // NODESTYLESETTINGDLG_H