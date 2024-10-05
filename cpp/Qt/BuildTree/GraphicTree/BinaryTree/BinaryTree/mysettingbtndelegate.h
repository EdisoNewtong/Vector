#pragma once


#include <QStyledItemDelegate>


class mysettingbtndelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	mysettingbtndelegate(QObject* parent = nullptr);

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

	// After Set Model data  :  ->   widget display data
    virtual void setEditorData(QWidget* editor, const QModelIndex &index) const override;
	// widget edited Done    :  ->   Model data
    virtual void setModelData(QWidget*  editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected slots:
	void onSettingBtnClick(bool checked = false);

protected:
    mutable QModelIndex  m_clickedBtnIdx;

};
