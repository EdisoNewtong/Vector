#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q11 : 

    有一个线性表 ( a1, a2, a3, ... , an), 其中 n >= 2, 采用带头结点的单链表存储，头指针为 L ,
每个结点存放线性表中的一个元素， 结点类型为 ( data, next ), 现查找某个元素值 == x 的结点指针，
若不存在这样的结点，则返回 NULL. 分别写出3种情况的查找语句，要求时间尽量小

1) 线性表中的元素无序
2) 线性表中的元素升序
3) 线性表中的元素降序
            
*/

typedef struct Node
{
    DataType data;
    struct Node* next;
} Node;

Node* generateLinkTable_Random()
{
    Node* head  = (Node*)malloc( sizeof(Node) );

    Node* node1 = (Node*)malloc( sizeof(Node) );
    node1->data = 10;

    Node* node2 = (Node*)malloc( sizeof(Node) );
    node2->data = -2;

    Node* node3 = (Node*)malloc( sizeof(Node) );
    node3->data = 7;

    Node* node4 = (Node*)malloc( sizeof(Node) );
    node4->data = 4;

    Node* node5 = (Node*)malloc( sizeof(Node) );
    node5->data = -9;
    
    head->next  = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = NULL;

    return head;
}

Node* searchNodeByValueInRandom(Node* head, DataType x)
{
    Node* searchResult = NULL;
    Node* cur = head->next;
    while ( cur != NULL    &&    !( searchResult = (cur->data != x ? NULL : cur) )   )
    {
        cur = cur->next;
    }

    return searchResult;
}

Node* generateLinkTable_AscendingOrder()
{
    Node* head  = (Node*)malloc( sizeof(Node) );

    Node* node1 = (Node*)malloc( sizeof(Node) );
    node1->data = -9;

    Node* node2 = (Node*)malloc( sizeof(Node) );
    node2->data = -2;

    Node* node3 = (Node*)malloc( sizeof(Node) );
    node3->data = 4;

    Node* node4 = (Node*)malloc( sizeof(Node) );
    node4->data = 7;

    Node* node5 = (Node*)malloc( sizeof(Node) );
    node5->data = 10;
    
    head->next  = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = NULL;

    return head;
}


Node* searchNodeByValueInAscendingOrder(Node* head, DataType x)
{
    Node* searchResult = NULL;
    Node* cur = head->next;
    while ( cur != NULL  )
    {
        if ( cur->data < x ) {
            cur = cur->next;
        } else {
            if ( cur->data == x ) { 
                searchResult = cur;
            }
            /* equal or  greater than,  either condtion will break */
            break;
        } 
    }

    return searchResult;
}


Node* generateLinkTable_DescendingOrder()
{
    Node* head  = (Node*)malloc( sizeof(Node) );

    Node* node1 = (Node*)malloc( sizeof(Node) );
    node1->data = 10;

    Node* node2 = (Node*)malloc( sizeof(Node) );
    node2->data = 7;

    Node* node3 = (Node*)malloc( sizeof(Node) );
    node3->data = 4;

    Node* node4 = (Node*)malloc( sizeof(Node) );
    node4->data = -2;

    Node* node5 = (Node*)malloc( sizeof(Node) );
    node5->data = -9;
    
    head->next  = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = NULL;

    return head;
}

Node* searchNodeByValueInDescendingOrder(Node* head, DataType x)
{
    Node* searchResult = NULL;
    Node* cur = head->next;
    while ( cur != NULL  )
    {
        if ( cur->data > x ) {
            cur = cur->next;
        } else {
            if ( cur->data == x ) { 
                searchResult = cur;
            }
            /* equal or  greater than,  either condtion will break */
            break;
        } 
    }

    return searchResult;
}



void printTb(const char* title, Node* head)
{
    printf("%s : \n\t", title);
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
        } else {
            printf(" -> NULL ");
        }
    }
    printf("\n");
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
    /* 10 , -2, 7, 4, -9 */
    Node* ranHead = generateLinkTable_Random();
    printTb("Random Link Table ", ranHead);

    Node* searchRan1 = searchNodeByValueInRandom(ranHead, 7);
    printf("search-Random for 7 : ");
    if ( searchRan1 ) { printf("%d\n", searchRan1->data); } else { printf(" NULL\n"); }

    Node* searchRan2 = searchNodeByValueInRandom(ranHead, 8);
    printf("search-Random for 8 : ");
    if ( searchRan2 ) { printf("%d\n", searchRan2->data); } else { printf(" NULL\n"); }
    printf("\n");



    Node* ascendingHead = generateLinkTable_AscendingOrder();
    printTb("Ascending-Order Link Table ", ascendingHead);

    Node* searchAsc1 = searchNodeByValueInAscendingOrder(ascendingHead, 4);
    printf("Search Ascending Order for 4 : ");
    if (  searchAsc1 ) { printf("%d\n", searchAsc1->data); } else { printf(" NULL\n"); }

    Node* searchAsc2 = searchNodeByValueInAscendingOrder(ascendingHead, 3);
    printf("Search Ascending Order for 3 : ");
    if ( searchAsc2 ) { printf("%d\n", searchAsc2->data); } else { printf(" NULL\n"); }
    printf("\n");


    Node* descendingHead = generateLinkTable_DescendingOrder();
    printTb("Descending-Order Link Table ", descendingHead);

    Node* searchDesc1 = searchNodeByValueInDescendingOrder(descendingHead, -2);
    printf("Search Descending Order for -2 : ");
    if (  searchDesc1 ) { printf("%d\n", searchDesc1->data); } else { printf(" NULL\n"); }

    Node* searchDesc2 = searchNodeByValueInDescendingOrder(descendingHead, 11);
    printf("Search Descending Order for 11 : ");
    if ( searchDesc2  ) { printf("%d\n", searchAsc2->data); } else { printf(" NULL\n"); }
    printf("\n");


    
    freeTable(ranHead);
    ranHead = NULL;
    freeTable(ascendingHead);
    ascendingHead = NULL;
    freeTable(descendingHead);
    descendingHead = NULL;

    return 0;
}
