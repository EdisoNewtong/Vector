#include "mytreeitem.h"

#include <QDebug>
#include <QBrush>

// static int G_CNT = 0;



mytreeitem::mytreeitem(const QString& text,bool isRoot /* = false */,bool canEdit /* = true*/, bool isValid /* = true */ )
    : QStandardItem(text)
    , m_bIsRoot(isRoot)
    , m_bIsValid(isValid)
{
    // qDebug() << "create a mytreeitem : " << text;// ++G_CNT;
    if (  !canEdit ) {
        auto oldflags = this->flags();
        this->setFlags(oldflags & (~Qt::ItemIsEditable) );
    }

    if ( !isValid ) {
        this->setData( QBrush(Qt::red), Qt::ForegroundRole );
    }
}

// virtual 
mytreeitem::~mytreeitem()
{
    // qDebug() << "delete a mytreeitem : " << this->text();   // --G_CNT;
}


bool mytreeitem::isRootNode()
{
    return m_bIsRoot;
}

bool mytreeitem::isValid()
{
    return m_bIsValid;
}
