#include "TreeView.h"

TreeView::TreeView(QWidget* parent /* = nullptr*/)
	: QTreeView(parent)
{
}


// virtual 
TreeView::~TreeView()
{
    auto model = this->model();
    if ( model != nullptr ) {
        delete model;
    }
    this->setModel(nullptr);
}



