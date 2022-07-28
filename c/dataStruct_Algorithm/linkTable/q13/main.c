#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q13 : 

   设计一个算法，将一个带头的数据依次为 a1, a2, a3, ... , an (n>=3) 的单链表的所有结点逆置
即每1个结点的数据域为an, 第2个结点的数据域变为 an-1 ...... , 尾结点的数据域变为 a1
            
*/

typedef struct Node
{
    DataType data;
    struct Node* next;
} Node;


Node* generateLinkTable(int cnt)
{
    Node* head  = (Node*)malloc( sizeof(Node) );
    head->data = -999;

    Node* cur = head;
    for ( int i = 0; i < cnt; ++i ) 
    {
        Node* newNode = (Node*)malloc( sizeof(Node) );
        newNode->data = (i + 1);

        cur->next = newNode;
        cur = newNode;
    }
    cur->next = NULL;

    return head;
}


void printTb(const char* title, Node* head)
{
    printf("%s\n\t", title);
    Node* cur = head;
    while ( cur != NULL )
    {
        if ( cur == head ) {
            printf("head ");
        } else {
            printf("%d ", cur->data);
        }

        cur = cur->next;
        if ( cur != NULL ) {
            printf(" -> ");
        } 
    }
    /* printf("\n"); */
}

void freeTable(Node* head)
{
    Node* cur = head;
    Node* saveNext = NULL;
    while ( cur != NULL )
    {
        saveNext = cur->next;
        free(cur);
        cur = saveNext;
    }
}


Node* getCenterNode(Node* head)
{
    Node* pMove1 = head->next;
    Node* pMove2 = head->next;

    /* 
        pMove2 move 2 step(s) every time
        pMove1 move 1 step    every time
        
    */
    while ( pMove2->next != NULL   &&   pMove2->next->next != NULL ) {
        pMove1 = pMove1->next;
        pMove2 = pMove2->next->next;
    }

    return pMove1;
}




int main(int argc, char* argv[])
{
    Node* tb4  = generateLinkTable(4);
    Node* tb5  = generateLinkTable(5);
    Node* tb6  = generateLinkTable(6);
    Node* tb7  = generateLinkTable(7);
    Node* tb8  = generateLinkTable(8);
    Node* tb9  = generateLinkTable(9);
    Node* tb10 = generateLinkTable(10);

    printTb("Table 4 : ",tb4);  printf("\n");
    Node* c4 = getCenterNode(tb4);
    if ( c4 != NULL ) { printf("\tCenter = %d\n", c4->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 5 : ",tb5);  printf("\n");
    Node* c5 = getCenterNode(tb5);
    if ( c5 != NULL ) { printf("\tCenter = %d\n", c5->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 6 : ",tb6);  printf("\n");
    Node* c6 = getCenterNode(tb6);
    if ( c6 != NULL ) { printf("\tCenter = %d\n", c6->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 7 : ",tb7);  printf("\n");
    Node* c7 = getCenterNode(tb7);
    if ( c7 != NULL ) { printf("\tCenter = %d\n", c7->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 8 : ",tb8);  printf("\n");
    Node* c8 = getCenterNode(tb7);
    if ( c8 != NULL ) { printf("\tCenter = %d\n", c8->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 9 : ",tb9);  printf("\n");
    Node* c9 = getCenterNode(tb9);
    if ( c9 != NULL ) { printf("\tCenter = %d\n", c9->data); } else { printf("\tCenter = NULL\n"); }

    printTb("Table 10 : ",tb10);  printf("\n");
    Node* c10 = getCenterNode(tb10);
    if ( c10 != NULL ) { printf("\tCenter = %d\n", c10->data); } else { printf("\tCenter = NULL\n"); }

    freeTable(tb4);  tb4 = NULL;
    freeTable(tb5);  tb5 = NULL;
    freeTable(tb6);  tb6 = NULL;
    freeTable(tb7);  tb7 = NULL;
    freeTable(tb8);  tb8 = NULL;
    freeTable(tb9);  tb9 = NULL;
    freeTable(tb10); tb10 = NULL;

    return 0;
}
