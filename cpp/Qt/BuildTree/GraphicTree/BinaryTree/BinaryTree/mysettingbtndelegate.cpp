#include "mysettingbtndelegate.h"

#include <QPushButton>
#include <QDebug>
#include <QColorDialog>
#include <QFontDialog>

#include "treenode.h"
#include "globalSettings.h"
#include "nodestylesettingdlg.h"

// static int G_CNT = 0;

mysettingbtndelegate::mysettingbtndelegate(QObject* parent /* = nullptr */ )
	: QStyledItemDelegate(parent)
{
    qDebug() << "Deleaget Created";
}

QWidget* mysettingbtndelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const // override
{
	if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX  ) {
        QPushButton* settingBtn = new QPushButton("Setting ... ", parent);
		m_clickedBtnIdx = index;
        connect( settingBtn, &QPushButton::clicked, this, &mysettingbtndelegate::onSettingBtnClick);

		return settingBtn;
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}



// After Set Model data  :  ->   widget display data
void mysettingbtndelegate::setEditorData(QWidget *editor, const QModelIndex &index) const // override;
{

	if ( editor != nullptr ) {
		if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
            // qDebug() << "setEditorData ( 2 ) ";
		} else {
            // qDebug() << "setEditorData ( not <2> ) ";
            // QStyledItemDelegate::setEditorData(editor, index);
		}
	}

    QStyledItemDelegate::setEditorData(editor, index);

}	



// widget edited Done    :  ->   Model data
void mysettingbtndelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const // override;
{
	if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
        // qDebug() << "setEditorData ( 2 ) ";
	} else {
        // qDebug() << "setEditorData ( not <2> ) ";

		QStyledItemDelegate::setModelData(editor, model, index);
	}

	// QStyledItemDelegate::setModelData(editor, model, index);
}	


void mysettingbtndelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const // override;
{

	if ( editor != nullptr ) {
		if ( index.column() == GlobalSetting::SPECIAL_COLUMN_INDEX ) {
            // qDebug() << "set Geometry ( 2 ) ";
			editor->setGeometry( option.rect );
		}  else {
            // qDebug() << "set Geometry ( Not 2 ) ";
		}
	}

	QStyledItemDelegate::updateEditorGeometry( editor, option, index );
}

void mysettingbtndelegate::onSettingBtnClick(bool checked /* = false */)
{
	(void)checked;

	if ( !m_clickedBtnIdx.isValid() ) {
		return;
	}

	treenode* currentNodeItem = static_cast<treenode*>( m_clickedBtnIdx.internalPointer() );
	if ( currentNodeItem == nullptr ) {
		return;
	}


    // bool bIsOK = false;
    // QFontDialog::getFont(&bIsOK, nullptr);
	
    auto dlg = new NodeStyleSettingDlg();
    dlg->init( currentNodeItem  );
    dlg->exec();

}
