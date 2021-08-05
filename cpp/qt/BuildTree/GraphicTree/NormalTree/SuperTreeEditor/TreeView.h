#ifndef Tree_View_H
#define Tree_View_H

#include <QTreeView>

class TreeView : public QTreeView
{
	Q_OBJECT
public:
    TreeView(QWidget* parent = nullptr);
    virtual ~TreeView();


};

#endif
