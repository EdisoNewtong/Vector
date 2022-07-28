#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q16 :

    设计一个双链表h, 每个结点中除了有 prior 、data 、next 几个域以外，
还有一个访问频度的域 freq , 在链表被启用之前，其值均初始化为 0
每当进行 LocateDNode(h,x) 运算时，令元素值为 x的结点中的freq 域值加1，
并调整表中的结点的次序，使其按访问频度的递减次序排序，以便使频繁访问的结点总是靠近表头。
试写一个符合上述要求的 LocateDNode 运算的算法
            
*/

typedef struct DNode
{
    DataType data;
    struct DNode* prev;
    struct DNode* next;
    int      freq;
} DNode;


DNode* generateLinkTable(int ary[], int arySize)
{
    DNode* head  = (DNode*)malloc( sizeof(DNode) );
    head->data = -9999999;
    head->prev   = NULL;
    head->next   = NULL;
    head->freq   = -1;

    DNode* cur = head;
    for( int i = 0; i < arySize; ++i ) {
        DNode* nd  = (DNode*)malloc( sizeof(DNode) );
        nd->data = ary[i];
        nd->freq = 0;
        nd->prev = cur;
        nd->next = NULL;

        cur->next = nd;
        cur = nd;
    }

    return head;
}


void printTb(const char* title, DNode* head)
{
    printf("%s\n\t", title);
    DNode* cur = head;
    while ( cur != NULL )
    {
        if ( cur == head ) {
            printf("head ");
        } else {
            printf("%2d(%2d) ", cur->data, cur->freq);
        }

        cur = cur->next;
        if ( cur != NULL ) {
            printf(" -> ");
        } 
    }
}




void freeTable(DNode* head)
{
    DNode* cur = head;
    DNode* saveNext = NULL;
    while ( cur != NULL )
    {
        saveNext = cur->next;
        free(cur);
        cur = saveNext;
    }
}




void LocateNode(DNode* head, DataType x)
{
    DNode* pXPosition = NULL;
    DNode* cur = head->next;
    int curFreq = -1;
    while ( cur != NULL )
    {
        if ( cur->data == x ) {
            curFreq = ++cur->freq;
            pXPosition = cur;
            break;
        }

        cur = cur->next;
    }


    DNode* preIt = pXPosition;
    while ( preIt != head ) 
    {
        if ( preIt->prev != head   ) {
            if ( preIt->prev->freq < curFreq ) {
                DataType v = preIt->data;
                preIt->data = preIt->prev->data;
                preIt->prev->data = v;

                preIt->freq = preIt->prev->freq;
                preIt->prev->freq = curFreq;
            } else {
                break;
            }
        }

        preIt = preIt->prev;
    }
}



int main(int argc, char* argv[])
{
    int ary[] = { -3, 5, -4, 100, 4, -6, -10, 2,  -9 };
    DNode* head  = generateLinkTable(ary, ( sizeof(ary)/ sizeof(ary[0]) ) );
    printTb("Original Link Table : ", head);
    printf("\n");

    LocateNode(head, 100);
    printTb("After Locate (100) Link Table : ", head);
    printf("\n\n");

    LocateNode(head, 4);
    printTb("After Locate (4) Link Table : ", head);
    printf("\n\n");
    LocateNode(head, 4);
    printTb("After Locate (4) Link Table : ", head);
    printf("\n\n");
    LocateNode(head, 4);
    printTb("After Locate (4) Link Table : ", head);
    printf("\n\n");

    LocateNode(head, 2);
    printTb("After Locate (2) Link Table : ", head);
    printf("\n\n");
    LocateNode(head, 2);
    printTb("After Locate (2) Link Table : ", head);
    printf("\n\n");


    LocateNode(head, 100);
    printTb("After Locate (100) Link Table : ", head);
    printf("\n\n");

    LocateNode(head, 100);
    printTb("After Locate (100) Link Table : ", head);
    printf("\n\n");

    LocateNode(head, 100);
    printTb("After Locate (100) Link Table : ", head);
    printf("\n\n");

    freeTable(head); head = NULL;

    return 0;
}

