#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q14 : 
    设计一个算法，在带头结点的非空单链表L中的第1个最大结点(最大值结点可能有多个) 之前插入一个值为 x 的结点
            
*/

typedef struct Node
{
    DataType data;
    struct Node* next;
} Node;


Node* generateLinkTable(int ary[], int arySize)
{
    Node* head  = (Node*)malloc( sizeof(Node) );
    head->data = -999;
    Node* cur = head;

    for( int i = 0; i < arySize; ++i ) {
        Node* nd  = (Node*)malloc( sizeof(Node) );
        nd->data = ary[i];
        cur->next = nd;
        cur = nd;
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


/*
    search maxNode's previous Node,  and insert element x
*/
void insertXBeforeMaxNode(Node* head, int x)
{
    Node* pre    = head;
    Node* maxPre = head;
    Node* maxNode = head->next;
    Node* cur = head->next;

    while ( cur != NULL )
    {
        if ( cur->data > maxNode->data ) {
            maxNode = cur;
            maxPre = pre;
        }

        cur = cur->next;
        pre = pre->next;
    }

    Node* pInsertNode = (Node*)malloc( sizeof(Node) );
    pInsertNode->data = x;
    pInsertNode->next = maxPre->next;
    maxPre->next = pInsertNode;
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







int main(int argc, char* argv[])
{
    /*                        / insert X here /                     rather than here                 */
    int ary[] = { -3, 5, -4,                    100, 4, -6, -10,                      100, 2, 4, -9 };
    Node* head  = generateLinkTable(ary, ( sizeof(ary)/ sizeof(ary[0]) ) );
    printTb("Original Link Table : ", head);
    printf("\n");

    const int x = -777;
    insertXBeforeMaxNode(head,x);
    printTb("Insert X Link Table : ", head);
    printf("\n");

    freeTable(head); head = NULL;

    return 0;
}
