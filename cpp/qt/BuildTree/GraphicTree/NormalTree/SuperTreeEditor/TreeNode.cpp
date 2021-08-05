#include "TreeNode.h"
#include <QDebug>

// static int G_CNT = 0;

TreeNode::TreeNode(const QString& text, TreeNode* parent)
	: m_text(text)
	, m_parent(parent)
{
	m_children.clear();
    // qDebug() << "Create G_CNT = " << ++G_CNT;
}



// virtual
TreeNode::~TreeNode()
{
	if ( m_parent != nullptr ) {
		if ( !m_parent->m_children.isEmpty() ) {
			int foundIdx = m_parent->m_children.indexOf( this );
			if ( foundIdx != -1 ) {
				// delete self inside the children list
				m_parent->m_children.removeAt(foundIdx);

				// cut parent relationship
				m_parent = nullptr;
			}
		}
	} else {
		//
		// this node is the <Root> Node , delete children is enough
		//
		for ( auto it = m_children.begin(); it != m_children.end(); ++it ) {
			auto node = *it;
			if ( node !=nullptr ) {
				node->m_parent = nullptr;
				delete node;
			}
			*it = nullptr;
		}
		m_children.clear();
	}

    // qDebug() << "Delete G_CNT = " << --G_CNT;
}


//
// insert at tail
//
TreeNode* TreeNode::appendChild()
{
	TreeNode* newCreateNode = new TreeNode( QString(""), this);
	m_children.push_back(newCreateNode);
	return newCreateNode;
}

//
// insert at head
//
TreeNode* TreeNode::prependChild()
{
	TreeNode* newCreateNode = new TreeNode( QString(""), this);
	m_children.push_front(newCreateNode);
	return newCreateNode;
}


TreeNode* TreeNode::insertSiblingNodeBefore()
{
	if ( m_parent != nullptr ) {
		if ( !m_parent->m_children.isEmpty() ) {
			int foundIdx = m_parent->m_children.indexOf( this );
			if ( foundIdx != -1 ) {
				TreeNode* newCreateNode = new TreeNode( QString(""), this->m_parent);
				m_parent->m_children.insert(foundIdx, newCreateNode);
				return newCreateNode;
			}
		}
	}

	return nullptr;
}

TreeNode* TreeNode::insertSiblingNodeAfter()
{
	if ( m_parent != nullptr ) {
		if ( !m_parent->m_children.isEmpty() ) {
			int foundIdx = m_parent->m_children.indexOf( this );
			if ( foundIdx != -1 ) {
				TreeNode* newCreateNode = new TreeNode( QString(""), this->m_parent);
				int insertPos = foundIdx + 1;
				if ( insertPos >= m_parent->m_children.size() ) {
					m_parent->m_children.push_back( newCreateNode);
				} else {
					m_parent->m_children.insert(insertPos, newCreateNode);
				}
				return newCreateNode;
			}
		}
	}

	return nullptr;
}

TreeNode* TreeNode::getParent()
{
	return m_parent;
}


int       TreeNode::selfIndex()
{
	if ( m_parent != nullptr ) {
		return m_parent->m_children.indexOf( this );
	}

	return 0;
}

QString   TreeNode::getText()
{
	return m_text;
}



int TreeNode::childCount()
{
	return m_children.size();
}

TreeNode* TreeNode::getChild(int idx)
{
	if ( idx >=0 && idx < m_children.size() ) {
		return m_children.at(idx);
	}

	return nullptr;
}


void      TreeNode::setText(const QString& txt)
{
	m_text = txt;
}


