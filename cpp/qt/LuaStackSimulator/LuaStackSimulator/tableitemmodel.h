#pragma once

#include <QAbstractTableModel>
#include <QVector>
// #include <QList>

struct RawData
{
	QString command;
	QString comment;
};

class TableItemModel : public QAbstractTableModel
{
public:
	TableItemModel(QObject* parent = nullptr);
	virtual ~TableItemModel();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const  Q_DECL_OVERRIDE;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
	virtual  Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;


	void addLine();
protected:

	QVector<RawData> m_innernalData;
};
