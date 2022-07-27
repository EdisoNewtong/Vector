#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 Q10 :
            在结点类型为 DLinkNode的双链表中， 将指定的 p 所指向的结点(非尾结点)  与 其后继结点交换的操作
*/

typedef struct DLinkNode
{
    DataType value;
    struct DLinkNode*  prev;
    struct DLinkNode*  next;
} DLinkNode;


void swapNodeWithNext(DLinkNode* p)
{
    if ( p == NULL  ||   p->next == NULL ) {
        return;
    }

    /*
             1           2            3            (4/NULL)
          p->prev        p         p->next
    */
    
    p->prev->next = p->next;
    p->next->prev = p->prev;

    p->prev = p->next;
    p->next = p->next->next;

    /*  Core Core Core , Easy to overlook(miss) the following chain info


          prev          p        next         next->next

        ---------   ---------  ---------      --------- 
        | node1 |   | node2 |  | node3 |      | node4 |    
        ---------   ---------  ---------      ---------    

        Easy to OverLook(miss) the following statement

      ##################################################
      #
      #    node4->prev = p;
      #
      ##################################################
    */
    if ( p->next != NULL ) {
        p->next->prev = p;
    }
    p->prev->next = p;
}

DLinkNode* generateLinkTable()
{
    DLinkNode* head = (DLinkNode*)malloc( sizeof(DLinkNode) );
    DLinkNode* n1   = (DLinkNode*)malloc( sizeof(DLinkNode) );
    n1->value = 10;
    DLinkNode* n2   = (DLinkNode*)malloc( sizeof(DLinkNode) );
    n2->value = 20;
    DLinkNode* n3   = (DLinkNode*)malloc( sizeof(DLinkNode) );
    n3->value = 30;
    DLinkNode* n4   = (DLinkNode*)malloc( sizeof(DLinkNode) );
    n4->value = 40;

    // gen next-chain
    head->next = n1;
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = NULL;

    // gen previous-chain
    head->prev = NULL;
    n4->prev = n3;
    n3->prev = n2;
    n2->prev = n1;
    n1->prev = head;
    
    return head;
}

void printTb(const char* title, DLinkNode* head)
{
    DLinkNode* cur  = head;
    DLinkNode* last = NULL;
    printf("%s\n", title);
    printf("  ");
    while ( cur != NULL ) {
        if ( cur == head ) {
            printf("head ");
        } else {
            printf("%d ", cur->value);
        }

        cur = cur->next;
        if ( cur != NULL ) {
            last = cur;
            printf(" -> ");
        } else {
            printf(" -> NULL");
        }
    }
    printf("\n  ");

    cur = last;
    while ( cur != NULL ) {
        if ( cur == head ) {
            printf("head ");
        } else {
            printf("%d ", cur->value);
        }

        cur = cur->prev;
        if ( cur != NULL ) {
            printf(" <- ");
        }
    }
    printf("\n");
}

void freeTable(DLinkNode* head)
{
    DLinkNode* cur = head;
    DLinkNode* save_next = NULL;
    while ( cur != NULL ) {
        save_next = cur->next;
        free(cur);
        cur = save_next;
    }
}



int main(int argc, char* argv[])
{
    DLinkNode* tb1 = generateLinkTable();
    printTb("Original Table 1 : ",tb1);
    swapNodeWithNext(tb1->next);
    printTb("Swap Table 1 (node1): ",tb1);
    printf("\n\n");
    
    DLinkNode* tb2 = generateLinkTable();
    printTb("Original Table 2 : ",tb2);
    swapNodeWithNext(tb2->next->next);
    printTb("Swap Table 2 (node2): ",tb2);
    printf("\n\n");

    DLinkNode* tb3 = generateLinkTable();
    printTb("Original Table 3 : ",tb3);
    swapNodeWithNext(tb3->next->next->next);
    printTb("Swap Table 3 ( node3 ): ",tb3);
    printf("\n\n");


    freeTable(tb1);
    tb1 = NULL;
    freeTable(tb2);
    tb2 = NULL;
    freeTable(tb3);
    tb3 = NULL;

    return 0;
}
