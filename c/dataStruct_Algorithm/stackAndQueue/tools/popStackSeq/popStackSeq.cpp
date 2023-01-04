#include <iostream>
#include <string>
#include <stack>
#include <list>
#include <iomanip>
#include <iterator>
using namespace std;


/********************************************************************************************************************************************

    问题 ： 

有 n 个元素序列 ， 分别是 ABCDEFG ...   , 对这些元素进行栈的操作，最终，当栈为空时，请问一共有多少种不同的输入序列
e.g.


----------------------------------------------------------------------------------------------------
    1个元素 A    : 那么栈的操作只有1种 : 

	    1.       A in --> A out :   因此出栈的序列也只有1种  A    

----------------------------------------------------------------------------------------------------
	2个元素 AB   : 那么栈的操作有以下2种 :

	    1.       A in --> A out --> B in  --> B out   :    AB
	    2.       A in --> B in  --> B out --> A out   :    BA


----------------------------------------------------------------------------------------------------
	3个元素 ABC   : 那么栈的操作有以下5种

	    1.       A in --> A out --> B in  --> B out --> C in   -->  C out  :     ABC
	    2.       A in --> A out --> B in  --> C in  --> C out  -->  B out  :     ACB
	    3.       A in --> B  in --> B out --> A out --> C in   -->  C out  :     BAC
	    4.       A in --> B  in --> C in  --> C out --> B out  -->  A out  :     CBA
	    5.       A in --> B  in --> B out --> C in  --> C out  -->  A out  :     BCA
	   
----------------------------------------------------------------------------------------------------


                n
总共有   :    C     /  (n+1)    种可能性
                2n


      n         (n+1)
=  C      -   C 
     2n         2n


第1个表达式表示 ： 在 2n 个槽中选中 n 个槽 ，向这些槽中填入n个 <入栈>的操作, 因此剩余的n个槽中只能填入 n个<出栈>操作，
<入栈> 与 <出栈> 是数量相等的, 但这其中有一些序列是<非法>的 栈 序列.
如: 
1. 出栈 
2. 入栈

<卡特兰>数


接下来让我们观察一下 n = 3 的一种出栈序列：+1, -1, -1, +1, -1, +1。序列<前三项之和>   小于 0，显然这是个非法的序列。
如果将 第1组 前缀和小于 0 的前缀，即前三项元素都进行取反，就会得到：-1, +1, +1, +1, -1, +1。此时有 (3+1) 个 +1 以及 (3-1) 个 -1。
因为这个小于 0 的前缀和必然是 -1，且 -1 比 +1 多一个，取反后，-1 比 +1 少一个，则 +1 变为 n + 1 个，且 -1 变为 n - 1 个。
进一步推广，对于 n 元素的每种非法出栈序列，都会对应一个含有 (n+1) 个 +1 以及 (n-1) 个 -1 的序列。

如何证明这两种序列是一一对应的?
假设非法序列为 A，
对应的序列为 B。

每个A只有一个"第一个前缀和小于 0 的前缀"，
所以每个A只能产生一个B。而每个B想要还原到A，就需要找到"第一个前缀和大于 0 的前缀"，显然 B 也只能产生一个 A。



每个 B 都有 (n+1) 个 +1 
       以及 (n-1) 个 -1，

                (n+1)
因此 B 的数量为C
                 2n

相当于在长度为 2n 的序列中找到(n+1)个位置存放 +1

                               (n+1)
相应的，非法序列的数量也就等于C
                                2n

						  n                                   n       (n+1)        n     /
因此出栈序列的总数量为 C       , 因此合法的出栈序列的数量为 C     -  C       =   C      /   (n+1)
                         2n                                  2n       2n          2n   /




2.2 括号序列

题目描述

n 对括号，则有多少种 “括号匹配” 的括号序列



思路:                                                           
                                                                n    /
左括号看成 +1，右括号看成 -1，那么就和上题的进出栈一样，共有  C     / (n+1)   种序列
                                                               2n  /


----------------------------------------------------------------------------------------------------



   这一过程 和 二叉树的结构极其地相似， 因为二叉树最多有2个结点 ， 而栈的操作也只有2种操作， 出栈/出栈
因此可以设计 一个二叉树的数据结构

这里约定:

    树的根结点一定是 Push 结点 , 树的叶子结点一定是 Pop 结点，同时，这一叶子结点也是 栈操作的 终结结点

       左子树为 Push 操作
	   右子树为 Pop  操作

l
********************************************************************************************************************************************/

struct BTreeNode
{
	static int G_REF_CNT;


	BTreeNode* pLeft;
	BTreeNode* pRight;

	BTreeNode() : pLeft(nullptr), pRight(nullptr) 
	{ 
		++G_REF_CNT;
		// cout << "create() : refCnt = " << G_REF_CNT
	}

	virtual ~BTreeNode() {
		if ( pLeft != nullptr ) {
			delete pLeft;
			pLeft = nullptr;
		}

		if ( pRight != nullptr ) {
			delete pRight;
			pRight = nullptr;
		}

		--G_REF_CNT;
	}
};


int BTreeNode::G_REF_CNT = 0;

void MakeBranch(BTreeNode* current,int PushCnt, int PopCnt, int elementCnt, list<int> operationList, const list<char>& inputSeq, int* pIdx);

void calcPopStackSeq_Recursively(const string& inputString)
{
	if ( inputString.empty() ) {
		return;
	}

	int elementCnt = static_cast<int>( inputString.size() );
	list<char> inSeq;
	for( int i = 0 ; i < elementCnt; ++i ) {
		inSeq.push_back( char('A' + i) );
	}

	BTreeNode* root = new BTreeNode; // root node must be a 'Push' Node

	int pushCnt = 1;
	int popCnt  = 0;
	list<int> operationList{ 1 };
	// list< list<int> > possibilites;
	int totalCnt = 0;
	MakeBranch(root, pushCnt, popCnt, elementCnt, operationList,  inSeq, &totalCnt);


	delete root;

	if ( BTreeNode::G_REF_CNT != 0 ) {
		cout << "==================================================" << endl;
		cout << "Memory Leak ,  BTreeNode::G_REF_CNT = " << BTreeNode::G_REF_CNT << endl;
		cout << "==================================================" << endl;
	} else {
		cout << "==================================================" << endl;
		cout << "Memory Alloc/Release Matched. " << endl;
		cout << "==================================================" << endl;
	}


	// After Build Binary Tree, 
	// Do <Travelsal> the tree till the <Leaf> node ( because the leaf node must be a 'Pop' node that makes the stack empty )
	// int idx = 0;
	// for( const auto& opActions : possibilites )
	// {
	// 	stack<char> stk;
	// 	int pushCnt = 0;
	// 	cout << setw(4) << (idx+1) << ".  ";
	// 	int j = 0;
	// 	for( auto it = opActions.begin(); it != opActions.end(); ++it, ++j ) {
	// 		auto act = *it;
	// 		cout << act << ( ( j < static_cast<int>( opActions.size() -1 ) ) ? " -> " : " " );

	// 		/*
	// 		if ( act == 1 ) { // push
	// 			stk.push( inSeq.at( pushCnt++ ) );
	// 		} else { // pop
	// 			if ( !stk.empty() ) {
	// 				auto top = stk.top();
	// 				stk.pop();
	// 				cout << top << ( (j < static_cast<int>(opActions.size() - 1)) ? " -> " : " ");
	// 			} else {
	// 				cout << "[ERROR] : @ Do Pop " << endl;
	// 			}
	// 		}
	// 		*/
	// 	}
	// 	cout << endl;


	// 	/*
	// 	if ( stk.empty() ) {
	// 		cout << endl;
	// 	} else {
	// 		cout << endl
	// 			 << "  [ERROR] : Stack is not empty()" << endl;
	// 	}
	// 	*/
	// 	
	// 	++idx;
	// }

}

void MakeBranch(BTreeNode* current,int PushCnt, int PopCnt, int elementCnt, list<int> operationList, const list<char>& inputSeq, int* pNCnt)
{
	if ( PopCnt == elementCnt ) {
		list<char> originalSeq(inputSeq);
		stack<char> stk;
		if ( pNCnt != nullptr ) {
			++(*pNCnt);
			cout << setw(4) << *pNCnt << ".  ";
		}

		// <word>11015. |E2|AC|86| => | ⬆ | ==> U+2B06</word>
		// <word>11016. |E2|AC|87| => | ⬇ | ==> U+2B07</word>

		// <word>10507. |E2|A4|8A| =&gt;   |   ⤊   | ==&gt; U+290A</word>
		// <word>10508. |E2|A4|8B| =&gt;   |   ⤋   | ==&gt; U+290B</word>


		// <word>8680. |E2|87|A7| =&gt;   |   ⇧   | ==&gt; U+21E7</word>
      	// <word>8682. |E2|87|A9| =&gt;   |   ⇩   | ==&gt; U+21E9</word>


		// <word>42703. |EA|9B|8E| =&gt;   |   ꛎ   | ==&gt; U+A6CE</word>
		// <word>42720. |EA|9B|9F| =&gt;   |   ꛟ   | ==&gt; U+A6DF</word>
		
		string operationRecord;
		int idx = 0;
		for ( auto it = operationList.begin(); it != operationList.end(); ++it, ++idx ) 
		{
			auto actionType = *it;
			if ( actionType == 1 ) { // Push
				if ( !originalSeq.empty() ) {
					char pushedElement = *(originalSeq.begin());
					stk.push( pushedElement );  
					originalSeq.erase( originalSeq.begin() );

					operationRecord += "\xEA\x9B\x9F"; // Push Down Symbol
					operationRecord += pushedElement;
					operationRecord += ", ";
				}
			} else { // Pop
				if ( !stk.empty() ) {
					char popedElement = stk.top();
					stk.pop();

					operationRecord += "\xEA\x9B\x8E"; // Pop Up Symbol
					operationRecord += popedElement;
					operationRecord += (idx < static_cast<int>( operationList.size()-1 ) ) ?  ", " : " ";

					cout << popedElement << ( idx < static_cast<int>(operationList.size()-1) ? " -> " : "");
				}
			}
		}

		if ( stk.empty() ) {
			cout << " ( " << operationRecord << " )" <<  endl;
		} else {
			cout << endl
				 << "	[ERROR] : After a push/pop , stack is not empty() " 
				 << endl;
		}

		return;
	}

	if ( PushCnt == elementCnt ) {
		current->pRight = new BTreeNode;

		list<int> copyRight( operationList );
		copyRight.push_back( 0 );
		// MakeBranch(current->pRight, PushCnt, PopCnt+1, elementCnt,  copyRight, possibilites);
		MakeBranch(current->pRight, PushCnt, PopCnt+1, elementCnt,  copyRight, inputSeq, pNCnt);
	} else {
		// PushCnt < elementCnt,    &&   PopCnt < elementCnt
		current->pLeft = new BTreeNode;
		list<int> copyLeft( operationList );
		copyLeft.push_back( 1 );
		// MakeBranch( current->pLeft, PushCnt+1, PopCnt, elementCnt, copyLeft, possibilites);
		MakeBranch( current->pLeft, PushCnt+1, PopCnt, elementCnt, copyLeft, inputSeq, pNCnt);

		if ( PushCnt > PopCnt ) {
			current->pRight = new BTreeNode;
			list<int> copyRight( operationList );
			copyRight.push_back( 0 );
			// MakeBranch( current->pRight, PushCnt, PopCnt+1, elementCnt, copyRight, possibilites);
			MakeBranch( current->pRight, PushCnt, PopCnt+1, elementCnt, copyRight, inputSeq, pNCnt);
		}
	}
}





void calcPopStackSeq_None_Recursively(const string& inputString)
{
	if ( inputString.empty() ) {
		return;
	}

	int elementCnt = static_cast<int>( inputString.size() );
	list<char> inSeq;
	for( int i = 0 ; i < elementCnt; ++i ) {
		inSeq.push_back( char('A' + i) );
	}

	list< pair<list<int>, pair<int,int> > > possibilityList;
	//                    first  : pushed count
	//                    second : poped  count
	possibilityList.push_back( make_pair(list<int>(), make_pair(0,0) ) );

	int slotCnt = elementCnt * 2;
	// int* actionSlotAry = new int[slotCnt];
	for( int i = 0; i < slotCnt; ++i  )
	{
		if ( i == 0 ) {
			for( auto it = possibilityList.begin(); it != possibilityList.end(); ++it ) {
				it->first.push_back(1);
				++(it->second.first); //  Pushed +1
			}
		} else if ( i == (slotCnt-1) ) {
			for( auto it = possibilityList.begin(); it != possibilityList.end(); ++it ) {
				it->first.push_back(0);
				++(it->second.second); //  Poped +1
			}
		} else {
			//
			// Neither  First  nor  Last
			//
			for( auto it = possibilityList.begin(); it != possibilityList.end();  ) {
				bool specialFlag = false;
				int previousPushed = it->second.first;
				int previousPoped  = it->second.second;

				if ( previousPushed > previousPoped ) {
					//
					// stack state is <NOT>   Empty
					//
					if ( previousPushed == elementCnt ) {
						// Only <Pop> Action is Valid
						it->first.push_back(0);
						it->second.second = previousPoped+1;
					} else {
						// previousPushed < elementCnt
						//	:  Both Push and Pop are valid
						// split 1 list into 2 duplicated list(s)

						// For Pop
						list<int> dup( it->first );
						dup.push_back( 0 );
						auto dupPr = it->second;
						++(dupPr.second);

						// For push
						it->first.push_back( 1 );
						++(it->second.first);

						// insert duplicate list after the current
						it = possibilityList.insert( ++it,  make_pair(dup, dupPr) );
						++it;
						specialFlag = true;
					}
				} else {
					// stack state is <Empty>
					// Only <Push> Action is valid
					it->first.push_back(1);
					it->second.first = previousPushed+1;
				}

				if ( !specialFlag ) {
					++it;
				}
			}
			
		}
	}

	cout << "Possibility.size = " << possibilityList.size() << endl;
	cout << endl;
	
	int idx = 0;
	for( auto it = possibilityList.begin(); it != possibilityList.end(); ++it, ++idx ) 
	{
		stack<char> stk;
		int j = 0;
		int pushIdx = 0;
		cout << setw(4) << (idx+1) << ". ";
		string operationRecord;
		for( auto e : it->first ) {
			if ( e == 1 ) { // push
				if ( pushIdx < static_cast<int>( inSeq.size() ) ) {
					auto it = inSeq.begin();
					advance(it, pushIdx++);
					stk.push( *it );

					operationRecord += "\xEA\x9B\x9F"; // Push <Down> Symbol
					operationRecord += *it;
					operationRecord += ", ";
				} else {
					cout << "	[ERROR] , advance Iterator is out of range, (" << pushIdx << ") times. " << endl;
				}
			} else { // pop
				if ( stk.empty() ) {
					cout << "	[ERROR] , Can't pop out of stack " << endl;
				} else {
					char topElement = stk.top();
					stk.pop();
					cout << topElement << ( (j < static_cast<int>(it->first.size()-1) ) ? " -> "  : " ");

					operationRecord += "\xEA\x9B\x8E"; // Pop <Up> Symbol
					operationRecord += topElement;
					operationRecord += (j < static_cast<int>( it->first.size()-1 ) ) ?  ", " : " ";
				}
			}

			++j;
		}
		cout << "	" << "( " << operationRecord << " )" << endl;

		if ( !stk.empty() ) {
			cout << "	" << "[ERROR] , stack is not empty " << endl;
		}

	}

}













int main(int argc, char* argv[], char* env[])
{
	if ( argc <= 1 ) {
		cout << "Missing args : " << endl;
		return -1;
	}

	// calcPopStackSeq_Recursively( string(argv[1]) );
	calcPopStackSeq_None_Recursively( string(argv[1]) );

	return 0;
}

