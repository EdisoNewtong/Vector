#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include <QByteArray>
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
	void addLineWithArgs(const QString& command, const QString& detail);
    bool removeLines(const QVector<int>& indexes, QString& error);

    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename, QString& error);

    void clearData();

protected:

	QVector<RawData> m_innernalData;

    static const QByteArray sc_VERSION;
    static const QByteArray sc_VERSION_VALUE;
    static const QByteArray sc_ENCODING;
    static const QByteArray sc_ENCODING_VALUE;

    static const QByteArray sc_rootName;
    static const QByteArray sc_nodeName;
    static const QByteArray sc_attrDetail;


private:
    class innernalUtil {
    public:
        innernalUtil(TableItemModel* base) : m_pBase(base) {
            if ( m_pBase != nullptr ) {
                m_pBase->beginResetModel();
            }
        }
        virtual  ~innernalUtil() {
            if ( m_pBase != nullptr ) {
                m_pBase->endResetModel();
            }
        }
    protected:
        TableItemModel* m_pBase;
    };

};
