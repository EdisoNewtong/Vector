#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q12 : 

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
        } else {
            printf(" -> NULL ");
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

void reverseTb(Node* head)
{
    Node* cur = head->next;
    head->next = NULL;

    while ( cur != NULL )
    {
        Node* next = cur->next;
        cur->next  = head->next;
        head->next = cur;

        cur = next;
    }
}



int main(int argc, char* argv[])
{
    Node* tb0 = generateLinkTable(0);
    Node* tb1 = generateLinkTable(1);
    Node* tb2 = generateLinkTable(2);
    Node* tb3 = generateLinkTable(3);
    Node* tb4 = generateLinkTable(4);

    printTb("Original Table 0 : ",tb0);  printf("\n");
    reverseTb(tb0);
    printTb("Reverse Table 0 : ",tb0);   printf("\n\n");

    printTb("Table 1 : ",tb1);           printf("\n");
    reverseTb(tb1);
    printTb("Reverse Table 1 : ",tb1);   printf("\n\n");

    printTb("Table 2 : ",tb2);           printf("\n");
    reverseTb(tb2);
    printTb("Reverse Table 2 : ",tb2);   printf("\n\n");

    printTb("Table 3 : ",tb2);           printf("\n");
    reverseTb(tb3);
    printTb("Reverse Table 3 : ",tb3);   printf("\n\n");

    printTb("Original Table 4 : ",tb4);  printf("\n");
    reverseTb(tb4);
    printTb("Reverse Table 4 : ",tb4);   printf("\n\n");

    
    freeTable(tb0);
    tb0 = NULL;
    freeTable(tb1);
    tb1 = NULL;
    freeTable(tb2);
    tb2 = NULL;
    freeTable(tb3);
    tb3 = NULL;
    freeTable(tb4);
    tb4 = NULL;

    return 0;
}
