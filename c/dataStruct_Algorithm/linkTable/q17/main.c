#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q17 :
    设 ha = (a1, a2, a3, ... , an)
       hb = (b1, b2, b3, ... , bm)

    是两个带头结点的循环单链表，设计一个算法将这两个表合并成 1个带头结点的循环单链表 hc ( a1, a2, a3, ... , an, b1, b2, b3, ... , bm )

*/

typedef struct CycleNode
{
    DataType data;
    struct CycleNode* next;
} CycleNode;


CycleNode* generateLinkTableCycle(int ary[], int arySize)
{
    CycleNode* head  = (CycleNode*)malloc( sizeof(CycleNode) );
    head->data = -9999999;
    head->next   = NULL;

    CycleNode* cur = head;
    for( int i = 0; i < arySize; ++i ) {
        CycleNode* nd  = (CycleNode*)malloc( sizeof(CycleNode) );
        nd->data = ary[i];
        nd->next = NULL;

        cur->next = nd;
        cur = nd;
    }
    cur->next = head;

    return head;
}


void printCycleTb(const char* title, CycleNode* head)
{
    printf("%s\n\t", title);
    CycleNode* cur = head->next;
    printf("head -> ");
    while ( cur != head )
    {
        printf("%2d ", cur->data);
        cur = cur->next;
        if ( cur != head ) {
            printf(" -> ");
        }
    }
}


CycleNode* merge2LinkTable_1( CycleNode* ha, CycleNode* hb)
{
    CycleNode* hc  = (CycleNode*)malloc( sizeof(CycleNode) );
    hc->data = -9999;
    CycleNode* cur = hc;

    CycleNode* curA = ha->next;
    do {
        cur->next = curA;
        cur = cur->next;

        curA = curA->next;
    } while ( curA != ha );

    CycleNode* curB = hb->next;
    do {
        cur->next = curB;
        cur = cur->next;

        curB = curB->next;
    } while ( curB != hb );

    // generate the chain info of the last element 
    cur->next = hc;

    return hc;
}


void merge2LinkTable_2( CycleNode* ha, CycleNode* hb)
{
    CycleNode* cur = ha;
    do {
        cur = cur->next;
    } while ( cur->next != ha );


    CycleNode* curB = hb;
    do {
        curB = curB->next;   
        cur->next = curB;
        cur = cur->next;
    } while ( curB->next != hb );

    // generate the chain info of the last element 
    cur->next = ha;

}


void freeCycleTable(CycleNode* head)
{
    CycleNode* cur = head;
    CycleNode* saveNext = NULL;

    do {
        saveNext = cur->next;
        free(cur);
        cur = saveNext;
    } while ( cur != head );
}



int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        printf("[ERROR] : Missing argument , 1 or 2 is valid\n");
        return -1;
    }

    int flag = atoi( argv[1] );
    if ( flag!=1  && flag!=2 ) {
        printf("[ERROR] : Argument's value is invalid  , 1 or 2 is valid\n");
        return -1;
    }

    int ary_a[] = { 1,2,3,4,5 };
    CycleNode* head_a  = generateLinkTableCycle(ary_a, ( sizeof(ary_a)/ sizeof(ary_a[0]) ) );
    printCycleTb("Original Linked Cycle Table A : ", head_a);
    printf("\n");

    int ary_b[] = { 0, -5, -4, -3, -2, -1 };
    CycleNode* head_b  = generateLinkTableCycle(ary_b, ( sizeof(ary_b)/ sizeof(ary_b[0]) ) );
    printCycleTb("Original Linked Cycle Table B : ", head_b);
    printf("\n\n");

    printf("flag = %d\n", flag);
    if ( flag == 1 ) {
        CycleNode* head_c = merge2LinkTable_1(head_a, head_b);
        printCycleTb("Merge Linked Cycle Table C ( with new head C ) : ", head_c);
        printf("\n");

        freeCycleTable(head_c); head_c = NULL;
        free(head_b);           head_b = NULL;
    } else { // flag == 2 
        merge2LinkTable_2(head_a, head_b);

        printCycleTb("Merge Linked Cycle Table C ( with old head A ): ", head_a);
        printf("\n");

        freeCycleTable(head_a); head_a = NULL;
        free(head_b);           head_b = NULL;
    }
    

    return 0;
}

