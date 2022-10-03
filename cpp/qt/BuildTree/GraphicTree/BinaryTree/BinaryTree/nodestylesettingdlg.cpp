#include "nodestylesettingdlg.h"
#include "ui_nodestylesettingdlg.h"

NodeStyleSettingDlg::NodeStyleSettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NodeStyleSettingDlg)
{
    ui->setupUi(this);
}

NodeStyleSettingDlg::~NodeStyleSettingDlg()
{
    delete ui;
}
