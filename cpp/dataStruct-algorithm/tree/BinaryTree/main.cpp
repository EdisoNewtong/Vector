#include <iostream>
#include <string>
#include <stack>

#include "treeDefine.h"
using namespace std;

extern Node* Build_TestCaseTree_0();
extern Node* Build_TestCaseTree_1();
extern Node* Build_TestCaseTree_2();
extern Node* Build_TestCaseTree_3();
extern Node* Build_TestCaseTree_4();
extern Node* Build_TestCaseTree_5();
extern Node* Build_TestCaseTree_6();
extern Node* Build_TestCaseTree_7();
extern Node* Build_TestCaseTree_8();
extern Node* Build_TestCaseTree_9();
extern Node* Build_TestCaseTree_10();
extern Node* Build_TestCaseTree_11();
extern Node* Build_TestCaseTree_12();
extern Node* Build_TestCaseTree_13();
extern Node* Build_TestCaseTree_14();
extern Node* Build_TestCaseTree_15();
extern Node* Build_TestCaseTree_16();
extern Node* Build_TestCaseTree_17();
extern Node* Build_TestCaseTree_18();

void PreOrder_Travelsal_Recursive(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

	// Root -> Left -> Right
	cout << root->val << " ";                     // travesal root    node
	PreOrder_Travelsal_Recursive(root->left);     // travesal left    node
	PreOrder_Travelsal_Recursive(root->right);    // travesal right   node
}


void PreOrder_Travelsal_NoneRecursive_v1(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

    stack<Node*> stk;
    stk.push( root );

	// Root -> Left -> Right
    do {
        auto currentNode = stk.top();
        cout << currentNode->val << " ";     // travesal root    node
        stk.pop();

        /***************************************************
         
          1st. You must push right-child first
          2nd. Then     push  left-child 

          Because the visit order is  1st. left -->  2nd right

          The **Stack** Container will visit them in the **Reverse** order

        ****************************************************/
        if ( currentNode->right != nullptr ) {
            stk.push( currentNode->right );
        }

        if ( currentNode->left != nullptr ) {
            stk.push( currentNode->left );
        }
    } while( !stk.empty() );
}

void PreOrder_Travelsal_NoneRecursive_v2(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

	// Root -> Left -> Right
    stack<Node*> stk;
    auto p = root;
    while ( !stk.empty() || p!=nullptr )
    {
        while( p!=nullptr ) {
            cout << p->val << " "; // visit top node
            stk.push(p);
            // try to push the subLeft-child tree
            p = p->left;
        }

        // After the previous while loop, the most left child node has been totally pushed into the stack
        //-------------------------------------------------------------------
        //      And the top node must have no subLeft-child tree
        //  It is a tree will only subRight-child or a leaf node
        //-------------------------------------------------------------------
        if ( !stk.empty() ) {
            p = stk.top();
            stk.pop();
            p = p->right;
        }
    }
}

void InOrder_Travelsal_Recursive(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

	// Left -> Root -> Right
	InOrder_Travelsal_Recursive(root->left);     // travesal left    node
	cout << root->val << " ";                     // travesal root    node
	InOrder_Travelsal_Recursive(root->right);    // travesal right   node
}


void InOrder_Travelsal_NoneRecursive(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

    stack<Node*> stk;
    auto pNode = root;

	// Left -> Root -> Right
    while( !stk.empty() || pNode!=nullptr )
    {
        while( pNode!=nullptr ) {
            stk.push(pNode);
            pNode = pNode->left;
        }

        if ( !stk.empty() ) {
            pNode = stk.top();
            cout << pNode->val << " ";
            stk.pop();

            pNode = pNode->right;
        }
    }
}

void PostOrder_Travelsal_Recursive(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

	// Left -> Right -> Root
	InOrder_Travelsal_Recursive(root->left);     // travesal left    node
	InOrder_Travelsal_Recursive(root->right);    // travesal right   node
	cout << root->val << " ";                    // travesal root    node
}

void PostOrder_Travelsal_NoneRecursive(Node* root)
{
	if ( root == nullptr ) {
		return;
	}

    stack<Node*> stk;
    auto pNode = root;

	// Left -> Root -> Right
    while( !stk.empty() || pNode!=nullptr )
    {
        while( pNode!=nullptr ) {
            stk.push( pNode );
            pNode = pNode->left;
        }

        Node* rightChild = nullptr;
        while( !stk.empty() )
        {
            pNode = stk.top();
            if ( pNode->right == rightChild ) {
                // the top element has !![NO]!! right-child
                stk.pop();
                cout << pNode->val << " ";
                rightChild = pNode;
            } else {
                // the top element has right-child
                pNode = pNode->right; // try to process the right-child
                break;
            }
        }
    }
}




int main()
{
    Node* example_trees[] = {
        Build_TestCaseTree_0(),
        Build_TestCaseTree_1(),
        Build_TestCaseTree_2(),
        Build_TestCaseTree_3(),
        Build_TestCaseTree_4(),
        Build_TestCaseTree_5(),
        Build_TestCaseTree_6(),
        Build_TestCaseTree_7(),
        Build_TestCaseTree_8(),
        Build_TestCaseTree_9(),
        Build_TestCaseTree_10(),
        Build_TestCaseTree_11(),
        Build_TestCaseTree_12(),
        Build_TestCaseTree_13(),
        Build_TestCaseTree_14(),
        Build_TestCaseTree_15(),
        Build_TestCaseTree_16(),
        Build_TestCaseTree_17(),
        Build_TestCaseTree_18(),
    };

    if ( Node::G_TRACE_MEMORY_LEAK ) { cout << "After BuildTree : G_NODE_CNT = " << Node::G_NODE_CNT << endl; }

    const auto CH_TAB = '\t';
    int idx = 0;
    for( auto tree : example_trees ) {
        cout << "Tree #" << idx << endl;
        cout << CH_TAB << "PreOrder Travelsal #1 : ";
        PreOrder_Travelsal_Recursive(tree);
        cout << endl;
        cout << CH_TAB << "PreOrder Travelsal #2 : ";
        PreOrder_Travelsal_NoneRecursive_v1(tree);
        cout << endl;
        cout << CH_TAB << "PreOrder Travelsal #3 : ";
        PreOrder_Travelsal_NoneRecursive_v2(tree);
        cout << endl << endl;

        cout << CH_TAB << "InOrder Travelsal #1 : ";
        InOrder_Travelsal_Recursive(tree);
        cout << endl;
        cout << CH_TAB << "InOrder Travelsal #2 : ";
        InOrder_Travelsal_NoneRecursive(tree);
        cout << endl << endl;

        cout << CH_TAB << "PostOrder Travelsal #1 : ";
        PostOrder_Travelsal_Recursive(tree);
        cout << endl;
        cout << CH_TAB << "PostOrder Travelsal #2 : ";
        PostOrder_Travelsal_NoneRecursive(tree);
        cout << endl << endl;

        ++idx;
    }

    // release memory
    for( auto& tree : example_trees ) {
        delete tree; tree = nullptr; 
    }

    if ( Node::G_TRACE_MEMORY_LEAK ) { cout << "After Release   : G_NODE_CNT = " << Node::G_NODE_CNT << endl; }

    return 0;
}



