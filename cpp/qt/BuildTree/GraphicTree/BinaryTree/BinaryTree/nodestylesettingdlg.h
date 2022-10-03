#ifndef NODESTYLESETTINGDLG_H
#define NODESTYLESETTINGDLG_H

#include <QDialog>

namespace Ui {
class NodeStyleSettingDlg;
}

class NodeStyleSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NodeStyleSettingDlg(QWidget *parent = nullptr);
    ~NodeStyleSettingDlg();

private:
    Ui::NodeStyleSettingDlg *ui;
};

#endif // NODESTYLESETTINGDLG_H
