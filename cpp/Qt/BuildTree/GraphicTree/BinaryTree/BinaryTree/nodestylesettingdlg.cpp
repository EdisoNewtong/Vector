#include "nodestylesettingdlg.h"
#include "ui_nodestylesettingdlg.h"

#include "nodestylecfg.h"
#include "globalSettings.h"

#include <QDebug>
#include <QFontMetricsF>
#include <QColorDialog>
#include <QFontDialog>
#include <QGraphicsEllipseItem>    // For Circle Item
#include <QGraphicsLineItem>       // Line Item
#include <QGraphicsSimpleTextItem> // Text Item

NodeStyleSettingDlg::NodeStyleSettingDlg(QWidget *parent) :
    QDialog(parent)
	, m_nodeToSet( nullptr )
	, m_bNeedUpdateAtOnce( false )
    , ui(new Ui::NodeStyleSettingDlg)
	
{
    ui->setupUi(this);
}

NodeStyleSettingDlg::~NodeStyleSettingDlg()
{
    delete ui;
}



void NodeStyleSettingDlg::init(treenode* node)
{
	if ( node == nullptr ) {
		return;
	}

	auto cfg = node->getNodeStyle();

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

	m_nodeToSet = node;
	m_originalCfg = cfg;
	m_toApplyCfg  = cfg;
}



/************************************************************
 
    UI Event

*************************************************************/

void NodeStyleSettingDlg::on_circleBrushSettingBtn_clicked()
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_toApplyCfg.m_circleBrushColor.color() , nullptr, tr("Pick your favourite the circle's brush color"), option);
	m_toApplyCfg.m_circleBrushColor = QBrush( pickedColor );

	ui->circleBrush->setStyleSheet( tr(" background-color : rgba(%1,%2,%3,%4); ")
			                        .arg( pickedColor.red() )
									.arg( pickedColor.green() )
									.arg( pickedColor.blue() )
									.arg( pickedColor.alpha() ) );

	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_circleBrushColor = m_toApplyCfg.m_circleBrushColor;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderCircleObject = m_nodeToSet->circleObject();
		if ( pRenderCircleObject != nullptr ) {
			pRenderCircleObject->setBrush( newCfg.m_circleBrushColor );
		}
	}
}

void NodeStyleSettingDlg::on_circlePenSettingBtn_clicked()
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_toApplyCfg.m_circlePen.color() , nullptr, tr("Pick your favourite the circle's brush color"), option);
    m_toApplyCfg.m_circlePen = QPen( QBrush(pickedColor), m_toApplyCfg.m_circlePen.widthF()  );

	ui->circlePen->setStyleSheet( tr(" background-color : rgba(%1,%2,%3,%4); ")
			                        .arg( pickedColor.red() )
									.arg( pickedColor.green() )
									.arg( pickedColor.blue() )
									.arg( pickedColor.alpha() ) );


	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_circlePen = m_toApplyCfg.m_circlePen;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderCircleObject = m_nodeToSet->circleObject();
		if ( pRenderCircleObject != nullptr ) {
			pRenderCircleObject->setPen( newCfg.m_circlePen );
		}
	}

}

void NodeStyleSettingDlg::on_textFontSettingBtn_clicked()
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

	auto newSelected = false;
	QFont newFnt = QFontDialog::getFont(&newSelected, m_toApplyCfg.m_textFont );
	if ( newSelected ) {
		m_toApplyCfg.m_textFont = newFnt;
		ui->label_4->setText( tr("Font : %1 %2").arg( m_toApplyCfg.m_textFont.family() ).arg( m_toApplyCfg.m_textFont.pointSize() ) );
	}

	if ( newSelected && m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_textFont = m_toApplyCfg.m_textFont;
		m_nodeToSet->setNodeStyle(newCfg);

		auto d = 2.0 * GlobalSetting::circle_radius;
		auto textRenderObject = m_nodeToSet->textObject();
		if ( textRenderObject != nullptr ) {
			auto fnt = newCfg.m_textFont;

			QFontMetricsF fmObj( fnt );
			auto dWidth = fmObj.horizontalAdvance( m_nodeToSet->text() );
			auto dHeight = fmObj.height();
			auto x = (d - dWidth) / 2.0;
			auto y = (d - dHeight) / 2.0;

			textRenderObject->setFont( newCfg.m_textFont );
			textRenderObject->setPos( QPointF(x,y) );
			textRenderObject->setText( m_nodeToSet->text() );
		}
	}

}

void NodeStyleSettingDlg::on_textBrushSettingBtn_clicked()
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_toApplyCfg.m_textBrush.color() , nullptr, tr("Pick your favourite the circle's brush color"), option);
	m_toApplyCfg.m_textBrush = QBrush( pickedColor );

	ui->textBrush->setStyleSheet(    tr(" background-color : rgba(%1,%2,%3,%4); ")
									.arg( pickedColor.red() )
									.arg( pickedColor.green() )
									.arg( pickedColor.blue() )
									.arg( pickedColor.alpha() ) );

	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_textBrush = m_toApplyCfg.m_textBrush;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderText = m_nodeToSet->textObject();
		if ( pRenderText != nullptr ) {
			pRenderText->setBrush( newCfg.m_textBrush );
		}
	}

}


void NodeStyleSettingDlg::on_connectionLineSettingBtn_clicked()
{

	if ( m_nodeToSet == nullptr ) {
		return;
	}

    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_toApplyCfg.m_connectionLinePen.color() , nullptr, tr("Pick your favourite the circle's brush color"), option);
    m_toApplyCfg.m_connectionLinePen = QPen( QBrush(pickedColor), m_toApplyCfg.m_connectionLinePen.widthF() );
	ui->linePen->setStyleSheet(     tr(" background-color  : rgba(%1,%2,%3,%4); ")
									.arg( pickedColor.red() )
									.arg( pickedColor.green() )
									.arg( pickedColor.blue() )
									.arg( pickedColor.alpha() )   );

	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_connectionLinePen = m_toApplyCfg.m_connectionLinePen;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderLine = m_nodeToSet->lineObject();
		if ( pRenderLine != nullptr ) {
			pRenderLine->setPen( newCfg.m_connectionLinePen );
		}
	}

}

void NodeStyleSettingDlg::on_circlePenWidthSetting_valueChanged(double arg1)
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

	qDebug() << "circlePen width changed : " << arg1;
	m_toApplyCfg.m_circlePen = QPen( m_toApplyCfg.m_circlePen.brush(), arg1 );

	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_circlePen = m_toApplyCfg.m_circlePen;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderCircleObject = m_nodeToSet->circleObject();
		if ( pRenderCircleObject != nullptr ) {
			pRenderCircleObject->setPen( newCfg.m_circlePen );
		}
	}


}

void NodeStyleSettingDlg::on_connectionLineWidthSetting_valueChanged(double arg1)
{

	if ( m_nodeToSet == nullptr ) {
		return;
	}

	qDebug() << "connection Line width changed : " << arg1;
	m_toApplyCfg.m_connectionLinePen = QPen( m_toApplyCfg.m_connectionLinePen.brush(), arg1 );

	if ( m_bNeedUpdateAtOnce ) {
		auto newCfg = m_nodeToSet->getNodeStyle();
		newCfg.m_connectionLinePen = m_toApplyCfg.m_connectionLinePen;
		m_nodeToSet->setNodeStyle(newCfg);

		auto pRenderLine = m_nodeToSet->lineObject();
		if ( pRenderLine != nullptr ) {
			pRenderLine->setPen( newCfg.m_connectionLinePen );
		}
	}

}

void NodeStyleSettingDlg::on_buttonBox_clicked(QAbstractButton *button)
{
	if ( button != nullptr ) {
		auto clickedBtnTag =  ui->buttonBox->standardButton(button);
		if ( clickedBtnTag == QDialogButtonBox::Ok ) {
			// qDebug() << "OK Clicked";
			m_nodeToSet->setNodeStyle( m_toApplyCfg );
		} else if ( clickedBtnTag == QDialogButtonBox::Cancel ) {
			// qDebug() << "Cancel Clicked";
			m_nodeToSet->setNodeStyle( m_originalCfg ); 
		} else if ( clickedBtnTag == QDialogButtonBox::Apply ) { 
			// qDebug() << "Apply Clicked";
			m_nodeToSet->setNodeStyle( m_toApplyCfg );
			updateAllRenderObject();
		}
	}

}




void NodeStyleSettingDlg::on_NodeStyleSettingDlg_finished(int result)
{
	(void)result;
    // qDebug() << "on finished : " << result;
	// if ( result == QDialog::Accepted ) {
	//
	// } else {
	// 	// QDialog::Rejected
	// }

	updateAllRenderObject();
}



void NodeStyleSettingDlg::updateAllRenderObject()
{
	if ( m_nodeToSet == nullptr ) {
		return;
	}

	auto newCfg = m_nodeToSet->getNodeStyle();
	auto pRenderCircleObject = m_nodeToSet->circleObject();
	if ( pRenderCircleObject != nullptr ) {
		pRenderCircleObject->setBrush( newCfg.m_circleBrushColor );
		pRenderCircleObject->setPen( newCfg.m_circlePen );
	}

	auto d = 2.0 * GlobalSetting::circle_radius;
	auto textRenderObject = m_nodeToSet->textObject();
	if ( textRenderObject != nullptr ) {
		auto fnt = newCfg.m_textFont;

		QFontMetricsF fmObj( fnt );
		auto dWidth = fmObj.horizontalAdvance( m_nodeToSet->text() );
		auto dHeight = fmObj.height();
		auto x = (d - dWidth) / 2.0;
		auto y = (d - dHeight) / 2.0;

		textRenderObject->setFont( newCfg.m_textFont );
		textRenderObject->setText( m_nodeToSet->text() );
		textRenderObject->setPos( QPointF(x,y) );

		textRenderObject->setBrush( newCfg.m_textBrush );
	}

	auto pRenderLine = m_nodeToSet->lineObject();
	if ( pRenderLine != nullptr ) {
		pRenderLine->setPen( newCfg.m_connectionLinePen );
	}



}