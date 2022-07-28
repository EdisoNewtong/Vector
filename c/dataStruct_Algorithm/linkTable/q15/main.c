#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q15 : 
    设计一个带头结点的非空单链表L， 结点的结构为(data,next)， 其中 data 为整数元素，
next为后继结点的指针。设计一个算法，首先按递减次序输出该单链表中各结点的数据元素，
然后释放所有结点占用的存储空间，并要求算法的空间复杂度为 O(1)
            
*/

typedef struct Node
{
    DataType data;
    struct Node* next;
} Node;


Node* generateLinkTable(int ary[], int arySize)
{
    Node* head  = (Node*)malloc( sizeof(Node) );
    head->data = -9999999;
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


/*
     [Alloc] a new header to store the sorted link-table
*/
Node* my_sortTable(Node* head)
{
    Node* ascendingOrderHead = (Node*)malloc( sizeof(Node) );
    ascendingOrderHead->data = - 99999;
    ascendingOrderHead->next = NULL;

    Node* cur = head->next;
    Node* sortPre = NULL;
    Node* sortCur = NULL;
    Node* savedNext = NULL;
    while ( cur != NULL )
    {
        savedNext = cur->next;
        sortPre = ascendingOrderHead;
        sortCur = ascendingOrderHead->next;

        if ( sortCur == NULL ) {
            // insert at 1st
            sortPre->next = cur;
            cur->next = NULL;
        } else {
            while ( sortCur != NULL    &&    sortCur->data > cur->data  ) 
            {
                sortPre = sortPre->next;
                sortCur = sortCur->next;
            }
            
            cur->next = sortPre->next;
            sortPre->next = cur;
        }

        cur = savedNext;
    }


    return ascendingOrderHead;
}

/*
     Sort in  Descending Order

     Do [Not] alloc new space to store the sorted link-table
*/
void collectAnswer_sortTable(Node* head)
{
    if ( head->next == NULL   ) {
        // empty link-table      //  or    single-node link-table
        printf("empty Link-Table, return \n");
        return;
    }

    // pointer at  the 2nd node
    Node* cmpNode = head->next->next;
    head->next->next = NULL; // cut off the next node chain

    while ( cmpNode != NULL )
    {
        Node* savedNext = cmpNode->next;

        Node* nodePre = head;
        Node* nodeIt  = head->next;
        while ( nodeIt!=NULL   &&   cmpNode->data < nodeIt->data )
        {
            nodeIt  = nodeIt->next;
            nodePre = nodePre->next;
        }
        cmpNode->next= nodePre->next;
        nodePre->next = cmpNode;

        cmpNode = savedNext;
    }
}





int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        printf("[ERROR] : Missing flag argument !! Type number 1/2 as the 1st argument of the program. \n");
        return -1;
    }

    /*                        / insert X here /                     rather than here                 */
    int ary[] = { -3, 5, -4,                    100, 4, -6, -10,                      100, 2, 4, -9 };
    Node* head  = generateLinkTable(ary, ( sizeof(ary)/ sizeof(ary[0]) ) );
    printTb("Original Link Table : ", head);
    printf("\n");

    Node* sortHead = NULL;
    int flag = atoi( argv[1] );
    if ( flag !=1   &&   flag !=2 ) {
        printf("[ERROR] : Sorry argument's value must be either 1 or 2\n");
        freeTable(head); head = NULL;
        return -1;
    }

    if ( flag == 1 ) {
        collectAnswer_sortTable(head);
        sortHead = head;
    } else { // flag == 2
        sortHead = my_sortTable(head);
    }

    printTb("Sorted  Link Table : ", sortHead);
    printf("\n");

    freeTable(head); head = NULL;
    if ( flag == 2 ) {
        free(sortHead);
        sortHead = NULL;
    }

    return 0;
}

