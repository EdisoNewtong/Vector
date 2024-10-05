#pragma once

#include <QFileInfo>


class myDirFileItem
{
public:
    enum E_ITEM_TYPE
    {
        E_INVISIBLE_ROOT = 0,

        E_FILE_EXT_ROOT,
         E_FILE_EXT_SORT,
          E_FILE_EXT_DETAIL,
           E_FILE_OBJECT,

        E_DIR_ROOT,
         E_DIR_SAME_ONE,
          E_DIR_OBJECT,
    };

    myDirFileItem(myDirFileItem* parent, myDirFileItem::E_ITEM_TYPE type, const QString& content);
    virtual ~myDirFileItem();

    myDirFileItem* getParent() const;
    E_ITEM_TYPE getType() const;
    QString getContent() const;

    virtual int getChildCount() const = 0;
protected:
    myDirFileItem* m_parent;
    E_ITEM_TYPE    m_type;

    QString        m_content;
};




