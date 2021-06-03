#ifndef VISIT_THREAD_H
#define VISIT_THREAD_H


#include <QThread>

class VisitThread : public QThread
{
	Q_OBJECT
public:
	VisitThread(QObject *parent = nullptr);
    // virtual ~VisitThread() = default;
	
	virtual void run() override;

	
	void setVisitPath(const QString &path);
	void setIncludeFlag(bool flag);
signals:
	void onStartVisit(const QString &dPath,int type, int level);
	void onVisitDone(int hasError);

	void onGetLevelItemCount(int count);
	void onFinishScanLevelItem(int nIndex);

protected:
	void visitDir(const QString &dpath,  int level);

	QString m_visitPath;
	bool    m_includeHiddenFlag;
};

#endif
