#include "mysettingbtndelegate.h"

#include <QPushButton>
#include <QDebug>
#include <QColorDialog>
#include <QFontDialog>


#include "globalSettings.h"
#include "nodestylesettingdlg.h"

mysettingbtndelegate::mysettingbtndelegate(QObject* parent /* = nullptr */ )
	: QStyledItemDelegate(parent)
{
    qDebug() << "Deleaget Created";
}

QWidget* mysettingbtndelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const // override
{
	if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX  ) {
        QPushButton* settingBtn = new QPushButton("Setting ... ", parent);
		// settingBtn->setFrame( false );
		m_clickedBtnIdx = index;
        connect( settingBtn, &QPushButton::clicked, this, &mysettingbtndelegate::onSettingBtnClick);
        // qDebug() << "createEditor (" << G_CNT << "). ";

		return settingBtn;
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}



// After Set Model data  :  ->   widget display data
void mysettingbtndelegate::setEditorData(QWidget *editor, const QModelIndex &index) const // override;
{

	if ( editor != nullptr ) {
		if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
            qDebug() << "setEditorData ( 2 ) ";
		} else {
            qDebug() << "setEditorData ( not <2> ) ";
            // QStyledItemDelegate::setEditorData(editor, index);
		}
	}

    QStyledItemDelegate::setEditorData(editor, index);

}	



// widget edited Done    :  ->   Model data
void mysettingbtndelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const // override;
{
	if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
		qDebug() << "setEditorData ( 2 ) ";
	} else {
		qDebug() << "setEditorData ( not <2> ) ";
		QStyledItemDelegate::setModelData(editor, model, index);
	}

	// QStyledItemDelegate::setModelData(editor, model, index);
}	


void mysettingbtndelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const // override;
{

	if ( editor != nullptr ) {
		if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
			qDebug() << "set Geometry ( 2 ) ";
			editor->setGeometry( option.rect );
		}  else {
			qDebug() << "set Geometry ( Not 2 ) ";
		}
	}

	QStyledItemDelegate::updateEditorGeometry( editor, option, index );
}

void mysettingbtndelegate::onSettingBtnClick(bool checked /* = false */)
{
	(void)checked;

	/*
	// QModelIndex
	qDebug() << "On Btn Clicked ";
	if ( !m_clickedBtnIdx.isValid() ) {
		qDebug() << "m_clickedBtnIdx is not valid.";
		return;
	}

	qDebug() << " row : " << m_clickedBtnIdx.row()
		     << " col : " << m_clickedBtnIdx.column();
	QModelIndex par = m_clickedBtnIdx.parent();
	if ( par.isValid() ) {
		qDebug() << "parent is OK. ";
	} else {
		qDebug() << "parent is not valid.";
	}

    qDebug() << "data = " << m_clickedBtnIdx.siblingAtColumn(1).data().toString();
	*/




    /*
	static int G_CNT = 0;
	++G_CNT;
    auto option = QColorDialog::ShowAlphaChannel;
    if ( G_CNT % 2 == 1 ) {
        qDebug() << "use option = ShowAlphaChannel";
        option = QColorDialog::ShowAlphaChannel;
    } else {
        qDebug() << "use option = DontUseNativeDialog";
        option = QColorDialog::DontUseNativeDialog;
    }
    QColorDialog::getColor( Qt::red, nullptr, tr("Pick your favourite the circle's brush color"), option);
    */

    // bool bIsOK = false;
    // QFontDialog::getFont(&bIsOK, nullptr);
	
    auto dlg = new NodeStyleSettingDlg();
    dlg->exec();

}
