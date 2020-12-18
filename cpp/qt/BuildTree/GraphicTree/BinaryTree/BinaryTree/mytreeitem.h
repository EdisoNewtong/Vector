#ifndef MY_TREE_ITEM_H
#define MY_TREE_ITEM_H

#include <QStandardItem>

class mytreeitem : public QStandardItem
{
public:
    mytreeitem(const QString& text, bool isRoot = false,bool canEdit = true,bool isValid = true);
    virtual ~mytreeitem();

    bool isRootNode();
    bool isValid();
protected:
    bool m_bIsRoot;
    bool m_bIsValid;
};


#endif
