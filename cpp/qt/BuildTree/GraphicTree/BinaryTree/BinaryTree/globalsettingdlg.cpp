#include "globalsettingdlg.h"
#include "ui_globalsettingdlg.h"

#include "globalSettings.h"
#include "globalSettingsLoadSave.h"

#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QFileDialog>


GlobalSettingDlg::GlobalSettingDlg(QWidget *parent /* = nullptr */) :
    QDialog(parent)
    , ui(new Ui::GlobalSettingDlg)
	//////////////////////////////////////////////////
	, m_sceneBg( GlobalSetting::scene_bg )
	//
	, m_circleBrush( GlobalSetting::circle_brush )
	, m_circlePen( GlobalSetting::circle_outline ) 
	, m_textFont( GlobalSetting::text_font )
	, m_textColor( GlobalSetting::text_color )
	, m_connectionLine( GlobalSetting::connection_line )
	//
	, m_circleRadius( GlobalSetting::circle_radius )
	, m_gap1( GlobalSetting::distance_between_leftright )
	, m_gap2( GlobalSetting::distance_between_right__left )
	, m_disHeight( GlobalSetting::height_between_parent_and_children )
	//
	, m_leftMargin( GlobalSetting::left_margin )
	, m_rightMargin( GlobalSetting::right_margin )
	, m_topMargin( GlobalSetting::top_margin )
	, m_bottomMargin( GlobalSetting::bottom_margin )
	//////////////////////////////////////////////////
{
    ui->setupUi(this);

	updateUI();
}

GlobalSettingDlg::~GlobalSettingDlg()
{
    delete ui;
}

void GlobalSettingDlg::on_circleBrushSettingBtn_clicked()
{
    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_circleBrush.color(), nullptr, tr("Pick your favourite circle's brush color"), option);
	m_circleBrush = QBrush(pickedColor);

	ui->circleBrush->setStyleSheet( tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_circleBrush.color().red() )
									 .arg( m_circleBrush.color().green() )
									 .arg( m_circleBrush.color().blue() )
									 .arg( m_circleBrush.color().alpha() ) );

}

void GlobalSettingDlg::on_circlePenSettingBtn_clicked()
{
    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_circlePen.color(), nullptr, tr("Pick your favourite circle's outline color"), option);
	m_circlePen = QPen( QBrush(pickedColor), m_circlePen.widthF() );

	ui->circlePen->setStyleSheet(   tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_circlePen.color().red() )
			                         .arg( m_circlePen.color().green() )
			                         .arg( m_circlePen.color().blue() )
			                         .arg( m_circlePen.color().alpha() )  );

}

void GlobalSettingDlg::on_textFontSettingBtn_clicked()
{
	auto newSelected = false;
	QFont newFnt = QFontDialog::getFont(&newSelected, m_textFont );
	if ( newSelected ) {
		m_textFont = newFnt;
	}
	ui->label_4->setText( tr("Font : %1 %2").arg( m_textFont.family() ).arg( m_textFont.pointSize() ) );

}

void GlobalSettingDlg::on_textBrushSettingBtn_clicked()
{
    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_textColor.color(), nullptr, tr("Pick your favourite text color"), option);
	m_textColor = QBrush( pickedColor );

	ui->textBrush->setStyleSheet(  tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_textColor.color().red() )
			                         .arg( m_textColor.color().green() )
			                         .arg( m_textColor.color().blue() )
			                         .arg( m_textColor.color().alpha() )  );
}

void GlobalSettingDlg::on_connectionLineSettingBtn_clicked()
{
    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_connectionLine.color(), nullptr, tr("Pick your favourite connection color"), option);
	m_connectionLine = QPen( QBrush(pickedColor), m_connectionLine.widthF() );

	ui->linePen->setStyleSheet(   tr(" background-color  : rgba(%1,%2,%3,%4); ")
			                       .arg( m_connectionLine.color().red() )
			                       .arg( m_connectionLine.color().green() )
			                       .arg( m_connectionLine.color().blue() )
			                       .arg( m_connectionLine.color().alpha() )   );

}

void GlobalSettingDlg::on_resetDefaultBtn_clicked()
{
	
	if ( QMessageBox::Yes != QMessageBox::question(this, "Warning ! ", "Are you sure reset setting to default ? ") ) {
		return;
	}

	GlobalSetting::resetToDefault();

	// update UI
	m_sceneBg = GlobalSetting::scene_bg;
	
	m_circleBrush = GlobalSetting::circle_brush;
	m_circlePen = GlobalSetting::circle_outline;
	m_textFont = GlobalSetting::text_font;
	m_textColor = GlobalSetting::text_color;
	m_connectionLine = GlobalSetting::connection_line;
	
	m_circleRadius = GlobalSetting::circle_radius;
	m_gap1 = GlobalSetting::distance_between_leftright;
	m_gap2 = GlobalSetting::distance_between_right__left;
	m_disHeight = GlobalSetting::height_between_parent_and_children;
	
	m_leftMargin = GlobalSetting::left_margin;
	m_rightMargin = GlobalSetting::right_margin;
	m_topMargin = GlobalSetting::top_margin;
	m_bottomMargin = GlobalSetting::bottom_margin;

	updateUI();
}

void GlobalSettingDlg::on_BgColorBtn_clicked()
{
    auto option = QColorDialog::ShowAlphaChannel;
    auto pickedColor = QColorDialog::getColor( m_sceneBg.color(), nullptr, tr("Pick your favourite text color"), option);
	m_sceneBg = QBrush( pickedColor );

    // Scene bg color
    ui->bgFrameColor->setStyleSheet( tr(" background-color  : rgba(%1,%2,%3,%4); ")
                                     .arg( m_sceneBg.color().red() )
                                     .arg( m_sceneBg.color().green() )
                                     .arg( m_sceneBg.color().blue() )
                                     .arg( m_sceneBg.color().alpha() )  );
}

void GlobalSettingDlg::on_circlePenWidthSetting_valueChanged(double arg1)
{
	m_circlePen = QPen( m_circlePen.brush() , arg1 );
}

void GlobalSettingDlg::on_connectionLineWidthSetting_valueChanged(double arg1)
{
	m_connectionLine = QPen( m_connectionLine.brush(), arg1 );
}



void GlobalSettingDlg::updateUI()
{
    ui->graphicImage->setStyleSheet( R"( background-image: url(:/images/demo.png);
background-repeat : no-repeat; )"  );

	ui->circleBrush->setStyleSheet( tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_circleBrush.color().red() )
									 .arg( m_circleBrush.color().green() )
									 .arg( m_circleBrush.color().blue() )
									 .arg( m_circleBrush.color().alpha() ) );

	ui->circlePen->setStyleSheet(   tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_circlePen.color().red() )
			                         .arg( m_circlePen.color().green() )
			                         .arg( m_circlePen.color().blue() )
			                         .arg( m_circlePen.color().alpha() )  );

	ui->circlePenWidthSetting->setValue( m_circlePen.widthF() );
	ui->label_4->setText( tr("Font : %1 %2").arg( m_textFont.family() ).arg( m_textFont.pointSize() ) );
	ui->textBrush->setStyleSheet(  tr(" background-color : rgba(%1,%2,%3,%4); ")
			                         .arg( m_textColor.color().red() )
			                         .arg( m_textColor.color().green() )
			                         .arg( m_textColor.color().blue() )
			                         .arg( m_textColor.color().alpha() )  );

	ui->linePen->setStyleSheet(   tr(" background-color  : rgba(%1,%2,%3,%4); ")
			                       .arg( m_connectionLine.color().red() )
			                       .arg( m_connectionLine.color().green() )
			                       .arg( m_connectionLine.color().blue() )
			                       .arg( m_connectionLine.color().alpha() )   );

	ui->connectionLineWidthSetting->setValue( m_connectionLine.widthF() );

	
	// Scene bg color
	ui->bgFrameColor->setStyleSheet( tr(" background-color  : rgba(%1,%2,%3,%4); ")
			                         .arg( m_sceneBg.color().red() )
			                         .arg( m_sceneBg.color().green() )
			                         .arg( m_sceneBg.color().blue() )
			                         .arg( m_sceneBg.color().alpha() )  );

	ui->lineEdit_1->setText( QString("%1").arg(m_circleRadius) );
	ui->lineEdit_2->setText( QString("%1").arg(m_gap1) );
	ui->lineEdit_3->setText( QString("%1").arg(m_gap2) );
	ui->lineEdit_4->setText( QString("%1").arg(m_disHeight) );

	ui->lineEdit_5->setText( QString("%1").arg(m_leftMargin) );
	ui->lineEdit_6->setText( QString("%1").arg(m_rightMargin) );
	ui->lineEdit_7->setText( QString("%1").arg(m_topMargin) );
	ui->lineEdit_8->setText( QString("%1").arg(m_bottomMargin) );

}

void GlobalSettingDlg::on_GlobalSettingDlg_finished(int result)
{
	if ( result == QDialog::Accepted ) {
		GlobalSetting::scene_bg = m_sceneBg;
		
		GlobalSetting::circle_brush = m_circleBrush;
		GlobalSetting::circle_outline = m_circlePen;
		GlobalSetting::text_font = m_textFont;
		GlobalSetting::text_color = m_textColor;
		GlobalSetting::connection_line = m_connectionLine;
		

		const int LINE_EDIT_CNT = 8;
		QLineEdit* lineEditAry[LINE_EDIT_CNT] = {
			ui->lineEdit_1,
			ui->lineEdit_2,
			ui->lineEdit_3,
			ui->lineEdit_4,
			ui->lineEdit_5,
			ui->lineEdit_6,
			ui->lineEdit_7,
			ui->lineEdit_8
		};


		auto meetError = false;
		QString errMsg;
		for( auto tag = 0; tag < LINE_EDIT_CNT; ++tag)
		{
			if ( !updateValueFromEditor( lineEditAry[tag], tag+1, errMsg) ) {
				meetError = true;
				break;
			}
		}

		if ( !meetError ) {
			GlobalSetting::circle_radius = m_circleRadius;
			GlobalSetting::distance_between_leftright = m_gap1;
			GlobalSetting::distance_between_right__left = m_gap2;
			GlobalSetting::height_between_parent_and_children = m_disHeight;
			
			GlobalSetting::left_margin = m_leftMargin;
			GlobalSetting::right_margin = m_rightMargin;
			GlobalSetting::top_margin = m_topMargin;
			GlobalSetting::bottom_margin = m_bottomMargin;
		} else {
			QMessageBox::critical(this, "Error", errMsg); 
		}

	}
}


bool GlobalSettingDlg::updateValueFromEditor(QLineEdit* lineEdit, int tag, QString& errorMsg)
{
	if ( lineEdit != nullptr ) {
		auto convertOK = false;
		auto txt = lineEdit->text();
		double dVal = 0.0;
		dVal = txt.toDouble(&convertOK);
		if ( convertOK ) {
			convertOK = (dVal > 0.0);
		}

		QString errMsg;
		switch( tag )
		{
		case 1:
			{
				errMsg = "Circle Radius must be a positive number.";
				if ( convertOK ) {
					m_circleRadius = dVal;
				}
			}
			break;
		case 2:
			{
				errMsg = "Gap-1 must be a positive number.";
				if ( convertOK ) {
					m_gap1 = dVal;
				}
			}
			break;
		case 3:
			{
				errMsg = "Gap-2 must be a positive number.";
				if ( convertOK ) {
					m_gap2 = dVal;
				}
			}
			break;
		case 4:
			{
				errMsg = "Vertical gap must be a positive number.";
				if ( convertOK ) {
					m_disHeight = dVal;
				}
			}
			break;
		case 5:
			{
				errMsg = "Left margin must be a positive number.";
				if ( convertOK ) {
					m_leftMargin = dVal;
				}
			}
			break;
		case 6:
			{
				errMsg = "Right margin must be a positive number.";
				if ( convertOK ) {
					m_rightMargin = dVal;
				}
			}
			break;
		case 7:
			{
				errMsg = "Top margin must be a positive number.";
				if ( convertOK ) {
					m_topMargin = dVal;
				}
			}
			break;
		case 8:
			{
				errMsg = "Bottom margin must be a positive number.";
				if ( convertOK ) {
					m_bottomMargin = dVal;
				}
			}
			break;
		default:
			break;
		}


		if ( !convertOK ) {
			errorMsg = errMsg;
		}

		return convertOK;
	} else {
		errorMsg = "LineEdit is nullptr";
		return false;
	}


}

void GlobalSettingDlg::on_loadBtn_clicked()
{
	auto loadFileName = QFileDialog::getOpenFileName(this,"Load Global Setting Config  ");
    if ( loadFileName.trimmed().isEmpty() ) {
        return;
    }

    QString errorMsg;
    auto ret = GSettingLoadSave::loadFromConfigFile(loadFileName, &errorMsg);
    if ( !ret ) {
        QMessageBox::critical(this, QStringLiteral("Load Global Render Setting Error"), errorMsg);
        return;
    }

    QMessageBox::information(this, QStringLiteral("Load Status"), QStringLiteral("TreeRender Config Load Successfully") );

	// update UI
	m_sceneBg = GlobalSetting::scene_bg;
	
	m_circleBrush = GlobalSetting::circle_brush;
	m_circlePen = GlobalSetting::circle_outline;
	m_textFont = GlobalSetting::text_font;
	m_textColor = GlobalSetting::text_color;
	m_connectionLine = GlobalSetting::connection_line;
	
	m_circleRadius = GlobalSetting::circle_radius;
	m_gap1 = GlobalSetting::distance_between_leftright;
	m_gap2 = GlobalSetting::distance_between_right__left;
	m_disHeight = GlobalSetting::height_between_parent_and_children;
	
	m_leftMargin = GlobalSetting::left_margin;
	m_rightMargin = GlobalSetting::right_margin;
	m_topMargin = GlobalSetting::top_margin;
	m_bottomMargin = GlobalSetting::bottom_margin;

	updateUI();
}

void GlobalSettingDlg::on_saveBtn_clicked()
{
    // bool GSettingLoadSave::saveToConfigFile(const QString& cfgFileName, QString* pErrorStr)
    auto savedfile = QFileDialog::getSaveFileName(this,"Save TreeRender Config" ,QString(), tr("XML files (*.xml)"));
    if ( savedfile.trimmed().isEmpty() ) {
        return;
    }

    QString errorMsg;
    auto ret = GSettingLoadSave::saveToConfigFile(savedfile, &errorMsg);
    if ( !ret ) {
        QMessageBox::critical(this, QStringLiteral("Saving Error"), errorMsg);
        return;
    } 
        
    QMessageBox::information(this, QStringLiteral("Saving Status"), QStringLiteral("TreeRender Config File Saved") );
}


