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



void NodeStyleSettingDlg::init(const nodeStyleCfg& cfg)
{
	// set the init state of the ui appearance
	const auto& circleBrush = cfg.m_circleBrushColor.color();
	const auto& circlePen   = cfg.m_circlePen.brush().color();
	const auto& textBrush   = cfg.m_textBrush.color();
    const auto& linePen     = cfg.m_connectionLinePen.brush().color();

	ui->circleBrush->setStyleSheet( tr(" background-color : rgba(%1,%2,%3,%4); ").arg( circleBrush.red() ).arg( circleBrush.green() ).arg( circleBrush.blue() ).arg( circleBrush.alpha() ) );
	ui->circlePen->setStyleSheet(   tr(" background-color : rgba(%1,%2,%3,%4); ").arg( circlePen.red() ).arg( circlePen.green() ).arg( circlePen.blue() ).arg( circlePen.alpha() )  );
	ui->circlePenWidthSetting->setValue( cfg.m_circlePen.widthF() );

	ui->label_4->setText( tr("Font : %1 %2").arg( cfg.m_textFont.family() ).arg( cfg.m_textFont.pointSize() ) );
	ui->textBrush->setStyleSheet(    tr(" background-color : rgba(%1,%2,%3,%4); ").arg( textBrush.red() ).arg( textBrush.green() ).arg( textBrush.blue() ).arg( textBrush.alpha() )   );

	ui->linePen->setStyleSheet(     tr(" background-color  : rgba(%1,%2,%3,%4); ").arg( linePen.red() ).arg( linePen.green() ).arg( linePen.blue() ).arg( linePen.alpha() )   );
	ui->connectionLineWidthSetting->setValue( cfg.m_connectionLinePen.widthF() );
}



/************************************************************
 
    UI Event

*************************************************************/

void NodeStyleSettingDlg::on_circleBrushSettingBtn_clicked()
{

}

void NodeStyleSettingDlg::on_circlePenSettingBtn_clicked()
{

}

void NodeStyleSettingDlg::on_textFontSettingBtn_clicked()
{

}



void NodeStyleSettingDlg::on_connectionLineSettingBtn_clicked()
{

}

void NodeStyleSettingDlg::on_textBrushSettingBtn_clicked()
{

}