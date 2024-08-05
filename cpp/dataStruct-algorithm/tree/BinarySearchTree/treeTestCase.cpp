#include "treeDefine.h"

// <Empty> tree
Node* Build_TestCaseTree_0()
{
    return nullptr;
}

// single Root tree
Node* Build_TestCaseTree_1()
{
	TreeNode* Root = new TreeNode(R"(a)");
    return Root;
}

// Root left-child only
Node* Build_TestCaseTree_2()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	Root->left = node_1;
    return Root;
}


// Root right-child only
Node* Build_TestCaseTree_3()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	Root->right = node_1;
    return Root;
}

// Root with left && right child only
Node* Build_TestCaseTree_4()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	Root->left  = node_1;
	Root->right = node_2;
    return Root;
}


// Root  -> left -> left tree
Node* Build_TestCaseTree_5()
{
	Node* Root = new TreeNode(R"(100)");
	Node* node_1 = new TreeNode(R"(50)");
	Node* node_2 = new TreeNode(R"(25)");
	Root->left  = node_1;
	node_1->left = node_2;

    return Root;
}

// Root  -> right -> right tree
Node* Build_TestCaseTree_6()
{
	Node* Root = new TreeNode(R"(100)");
	Node* node_1 = new TreeNode(R"(50)");
	Node* node_2 = new TreeNode(R"(25)");
	Root->right  = node_1;
	node_1->right = node_2;

    return Root;
}

/****************************************************************

    Complex Tree Part

****************************************************************/
Node* Build_TestCaseTree_7()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;

    return Root;
}

Node* Build_TestCaseTree_8()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;
	TreeNode* node_4 = new TreeNode(R"(e)");
	Root->right = node_4;

    return Root;
}

Node* Build_TestCaseTree_9()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	node_3->right = node_4;
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;

    return Root;
}

Node* Build_TestCaseTree_10()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	node_3->left = node_4;
	TreeNode* node_5 = new TreeNode(R"(f)");
	node_3->right = node_5;
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;


    return Root;
}


Node* Build_TestCaseTree_11()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	TreeNode* node_5 = new TreeNode(R"(f)");
	node_4->right = node_5;
	node_3->right = node_4;
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;

    return Root;
}


Node* Build_TestCaseTree_12()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	TreeNode* node_5 = new TreeNode(R"(f)");
	node_4->right = node_5;
	node_3->right = node_4;
	node_2->right = node_3;
	node_1->left = node_2;
	Root->left = node_1;

    return Root;
}


Node* Build_TestCaseTree_13()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	node_2->left = node_3;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}


Node* Build_TestCaseTree_14()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	node_2->left = node_3;
	TreeNode* node_4 = new TreeNode(R"(e)");
	node_2->right = node_4;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}

Node* Build_TestCaseTree_15()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	TreeNode* node_5 = new TreeNode(R"(f)");
	TreeNode* node_6 = new TreeNode(R"(g)");
	node_5->left = node_6;
	node_4->left = node_5;
	node_3->left = node_4;
	node_2->left = node_3;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}


Node* Build_TestCaseTree_16()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	TreeNode* node_5 = new TreeNode(R"(f)");
	TreeNode* node_6 = new TreeNode(R"(g)");
	node_5->right = node_6;
	node_4->right = node_5;
	node_3->right = node_4;
	node_2->left = node_3;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}


Node* Build_TestCaseTree_17()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	node_2->left = node_3;
	TreeNode* node_4 = new TreeNode(R"(e)");
	node_2->right = node_4;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}


Node* Build_TestCaseTree_18()
{
	TreeNode* Root = new TreeNode(R"(a)");
	TreeNode* node_1 = new TreeNode(R"(b)");
	TreeNode* node_2 = new TreeNode(R"(c)");
	TreeNode* node_3 = new TreeNode(R"(d)");
	TreeNode* node_4 = new TreeNode(R"(e)");
	TreeNode* node_5 = new TreeNode(R"(f)");
	TreeNode* node_6 = new TreeNode(R"(g)");
	node_5->right = node_6;
	node_4->right = node_5;
	node_3->right = node_4;
	node_2->left = node_3;
	node_1->right = node_2;
	Root->right = node_1;

    return Root;
}



