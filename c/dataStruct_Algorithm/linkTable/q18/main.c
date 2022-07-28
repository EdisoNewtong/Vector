#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q18 :
    
     设两个非空线性表分别用带头结点的循环双链表 ha 和 hb 表示， 设计一个算法
                                     ----------
Insert(ha, hb, i), 其功能是: 

 当 i = 0 时，             将 hb 插入到 ha 的前面
 当 i > 0 且 < ha.size 时，将 hb 插入到 ha 的第 i 个结点的后面
 当 i >= ha.size 时,       将 hb 插入到 ha 的后面
                         


*/

typedef struct CycleDNode
{
    DataType data;
    struct CycleDNode* prev;
    struct CycleDNode* next;
} CycleDNode;


CycleDNode* generateLinkTableCycle(int ary[], int arySize)
{
    CycleDNode* head  = (CycleDNode*)malloc( sizeof(CycleDNode) );
    head->data = -9999999;
    head->prev = NULL;
    head->next = NULL;

    CycleDNode* cur = head;
    for( int i = 0; i < arySize; ++i ) {
        CycleDNode* nd  = (CycleDNode*)malloc( sizeof(CycleDNode) );
        nd->data = ary[i];
        nd->prev = cur;
        nd->next = NULL;

        cur->next = nd;
        cur = nd;
    }

    // make linked list Cycle
    head->prev = cur;
    cur->next  = head;

    return head;
}


void printCycleTb(const char* title, CycleDNode* head)
{
    printf("%s\n\t", title);
    CycleDNode* cur = head->next;
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


void insertCycleTable(CycleDNode* ha, CycleDNode* hb, int position)
{
    // calc length of   ha
    int length_a = 0;
    CycleDNode* cur = ha->next;
    while ( cur != ha )
    {
        ++length_a;
        cur = cur->next;
    }

    if ( position == 0 ) {

        /* 
             insert              cycle-B     <Before>        cycle-A

                              -------------               -------------
             Head-A   -->     |  Cycle-B  |     -->       |  Cycle-A  |
                              -------------               -------------
        */
        CycleDNode* node_a = ha->next;
        CycleDNode* node_b = hb->next;

        ha->next = node_b;
        node_b->prev = ha;

        hb->prev->next = node_a;
        node_a->prev = hb->prev;
    } else if ( position < length_a ) {
        CycleDNode* node_a = ha;
        CycleDNode* node_aNext = NULL;
        int idx = 0;
        while ( idx < position )
        {
            ++idx;
            node_a = node_a->next;
        }
        node_aNext = node_a->next;

        node_a->next = hb->next;
        hb->next->prev = node_a;

        hb->prev->next   = node_aNext;
        node_aNext->prev = hb->prev;
    } else {
        /* 
             insert              cycle-B     <After>        cycle-A

                              -------------               -------------
             Head-A   -->     |  Cycle-A  |     -->       |  Cycle-B  |
                              -------------               -------------
        */

        ha->prev->next = hb->next;
        hb->next->prev = ha->prev;

        hb->prev->next = ha;
        ha->prev       = hb->prev;
    }
    
}


void freeCycleTable(CycleDNode* head)
{
    CycleDNode* cur = head;
    CycleDNode* saveNext = NULL;

    do {
        saveNext = cur->next;
        free(cur);
        cur = saveNext;
    } while ( cur != head );
}



int main(int argc, char* argv[])
{
    if ( argc < 2 ) {
        printf("[ERROR] : Missing argument , require a none-negative number. \n");
        return -1;
    }

    int position = atoi( argv[1] );
    if ( position < 0 ) {
        printf("[ERROR] : Invalid argument , require a none-negative number. \n");
        return -1;
    }

    int ary_a[] = { 1,2,3,4,5 };
    CycleDNode* head_a  = generateLinkTableCycle(ary_a, ( sizeof(ary_a)/ sizeof(ary_a[0]) ) );
    printCycleTb("Original Linked Cycle Table A : ", head_a);
    printf("\n");

    int ary_b[] = { 0, -5, -4, -3, -2, -1 };
    CycleDNode* head_b  = generateLinkTableCycle(ary_b, ( sizeof(ary_b)/ sizeof(ary_b[0]) ) );
    printCycleTb("Original Linked Cycle Table B : ", head_b);
    printf("\n");

    insertCycleTable(head_a, head_b, position);
    printCycleTb("Insert Linked Cycle Table B at position (...) : ", head_a);
    printf("\n");


    freeCycleTable(head_a);   head_a = NULL;
    free(head_b);             head_b = NULL;

    return 0;
}

