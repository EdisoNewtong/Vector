#include <stdio.h>
#include <stdlib.h>


static int g_allocCnt = 0;
static int g_freeCnt = 0;

/***************************************************
  Generalist List
****************************************************/


typedef struct GeneralistNode
{
	// 0: Child node is a single element ( 'data'    need to be filled )   rather than a sublist
	// 1: Child node is a table          ( 'sublist' need to be filled )   rather than a data    ( Can be an empty table )
	int childTag;  //  Downer Child Type
	union {
		char data;                       // Element data
		struct GeneralistNode* sublist;  // Sub list
	} val; // Downer Data Info
	struct GeneralistNode*   nextSiblingNode; // (next) right sibling node
} GeneralistNode;


/***************************************************
 
                       -----
                       | D |
                       -----
                      /  |  \
					 /   |   \
					/    |    \
				   /     |     \
				  /      |      \
                -----  -----   -----
                | A |  | B |   | C |
			    -----  -----   -----
				         |       /\
				         |      /  \
						 e     a   ------
                                   |    |
                                   ------
                                   / | \
                                  /  |  \
                                 /   |   \
                                b    c    d

****************************************************/

void initNode(GeneralistNode* node)
{
	if ( node != NULL ) {
		node->val.sublist = NULL;
		node->nextSiblingNode = NULL;
	}
}


GeneralistNode* create_GeneralistTable()
{
	// Node 'D' ==> rootNode
	GeneralistNode* rootNode = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(rootNode); ++g_allocCnt;

	GeneralistNode* node_A   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_A); ++g_allocCnt;
	GeneralistNode* node_B   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_B); ++g_allocCnt;
	GeneralistNode* node_C   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_C); ++g_allocCnt;

	GeneralistNode* node_a   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_a); ++g_allocCnt;
	GeneralistNode* node_anonymous   = (GeneralistNode*)malloc( sizeof(GeneralistNode) ); initNode(node_anonymous);   ++g_allocCnt;
	GeneralistNode* node_b   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_b); ++g_allocCnt;
	GeneralistNode* node_c   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_c); ++g_allocCnt;
	GeneralistNode* node_d   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_d); ++g_allocCnt;
	GeneralistNode* node_e   = (GeneralistNode*)malloc( sizeof(GeneralistNode) );  initNode(node_e); ++g_allocCnt;


	rootNode->childTag = 1; rootNode->val.sublist = node_A;
	node_A->childTag   = 1; node_A->val.sublist = NULL;    node_A->nextSiblingNode = node_B;
	node_B->childTag   = 1; node_B->val.sublist = node_e;  node_B->nextSiblingNode = node_C;
	  node_e->childTag = 0; node_e->val.data = 'e'; // node_e->nextSiblingNode = node_a;  // NOTES : [ERROR]  node_a is not the next sibling node of node_B because they have different parent
	node_C->childTag   = 1; node_C->val.sublist = node_a;
	  node_a->childTag = 0; node_a->val.data = 'a';        node_a->nextSiblingNode = node_anonymous;
	  node_anonymous->childTag = 1; node_anonymous->val.sublist = node_b;
	    node_b->childTag = 0; node_b->val.data = 'b'; node_b->nextSiblingNode = node_c;
	    node_c->childTag = 0; node_c->val.data = 'c'; node_c->nextSiblingNode = node_d;
	    node_d->childTag = 0; node_d->val.data = 'd'; // END

	return rootNode;

}


void display_GeneralistTable(GeneralistNode* rootNode, int layer)
{
	if ( rootNode == NULL ) {
		return;
	}

	// rootNode is not NULL
	if ( rootNode->childTag == 0 ) {
		printf("%c", rootNode->val.data);
	} else {
		printf("("); // begin travelsal Sub generalist Table
		display_GeneralistTable(rootNode->val.sublist, layer+1);
		printf(")");
	}

	if ( rootNode->nextSiblingNode != NULL ) {
		printf(",");
		display_GeneralistTable(rootNode->nextSiblingNode, layer);
	}

}


void destory_GeneralistTable(GeneralistNode* rootNode)
{
	if ( rootNode == NULL ) {
		return;
	}

	int childTag = rootNode->childTag;
	GeneralistNode* sub = rootNode->val.sublist;
	GeneralistNode* next = rootNode->nextSiblingNode;

    // destory next sibling node
    destory_GeneralistTable( next );
    if ( childTag == 1 ) {
        // destory sub
        destory_GeneralistTable( sub );
    }
    ++g_freeCnt;  free(rootNode);

}



int main(int argc, char* argv[])
{

	GeneralistNode* rootNode = create_GeneralistTable();
	printf("g_alloccnt = %d\n", g_allocCnt);
	display_GeneralistTable(rootNode, 0);
	printf("\n");
	destory_GeneralistTable( rootNode );
	printf("g_freeCnt = %d\n", g_freeCnt);

	return 0;
}
