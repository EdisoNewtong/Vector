#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q20 :
      用带头结点的单链表，表示整数集合，完成以下算法并分析时间复杂度  
                         
1) 设计一个算法，求 A - B ,  C = A - B ,   要求算法的空间复杂度为 O(1), 并释放单链表 A和B 中不需要的结点
2) 假设集合中的元素按递增排列,  设计一个高效算法求两个集体 A-B 的差集运算 , C = A - B,  并释放单链表 A 和 B 中不需要的结点


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
    head->next = NULL;

    Node* cur = head;
    for( int i = 0; i < arySize; ++i ) {
        Node* nd  = (Node*)malloc( sizeof(Node) );
        nd->data = ary[i];
        nd->next = NULL;

        cur->next = nd;
        cur = nd;
    }

    cur->next  = NULL;

    return head;
}


void printTb(const char* title, Node* head)
{
    printf("%s\n\t", title);
    Node* cur = head->next;
    printf("head -> ");
    while ( cur != NULL )
    {
        printf("%2d ", cur->data);
        cur = cur->next;
        if ( cur != NULL ) {
            printf(" -> ");
        }
    }
}





void freeTable(Node* head)
{
    Node* cur = head;
    Node* saveNext = NULL;

    do {
        saveNext = cur->next;
        free(cur);
        cur = saveNext;
    } while ( cur != NULL );
}


void Subscract2LinkTableWithRandomOrder()
{
    int ary_a[] = { 1,3,2,5,4 };
    Node* head_a  = generateLinkTable(ary_a, ( sizeof(ary_a)/ sizeof(ary_a[0]) ) );
    printTb("Original Linked Table A : ", head_a);
    printf("\n");

    int ary_b[] = { 3, 0, 6, 5 };
    /* int ary_b[] = { 7,6 }; */
    Node* head_b  = generateLinkTable(ary_b, ( sizeof(ary_b)/ sizeof(ary_b[0]) ) );
    printTb("Original Linked Table B : ", head_b);
    printf("\n");

    // Calc   A - B
    Node* nb = head_b->next;
    while ( nb != NULL )
    {
        Node* naPre = head_a;
        Node* na    = head_a->next;
        while ( na != NULL ) 
        {
            if ( nb->data == na->data ) {
                // delete na
                naPre->next = na->next;
                free(na);
                break;
            }

            naPre = naPre->next;
            na = na->next;
        }

        nb = nb->next;
    }


    printTb("C = A - B  : ", head_a);
    printf("\n");

    freeTable(head_a);
    freeTable(head_b);
}

void SubstractLinkTableWithAscendingOrder()
{

    /******************************************************************************************************
     
        由于 Table-A 与 Table-B 都是从小到大， 升序进行排列的，因此需要利用这一特性
        
            e.g 1.
                    A : 1,3,5,    7, 8,9
                    B : 6,8


            1. 当在 A 中，查找B的元素 6 是否存在时，A的元素访问指针，从左向右遍历，经过1,3,5后，当访问到 7 时, 因为 6 < 7 ，而 7 后面都是比 7 大的数字，并不可能再找到 6 这个数字，因此 !!!中止!!! 遍历
            2. 当在 A 中，查找B的元素 8 是否存在时，A的元素访问指针，不必再从头开始遍历，因为 8 > 6 , 在 Step1 查找 6 时， 已经掠过了1,3,5 , 那么 8 也一定满足掠过 1,3, 5的条件，因此，8 需要从 6 后面的那个数字开始遍历
          

        类似的 : 
            e.g 1.
                    A : 1,3,5,    7, 8,9
                    B : -2, 1

                当-2 < 1 时，就无需再移动A的遍历指针了，因为 1 后面的数字都比 1 要大

        因此，从上面的分析，可以很明显地得出，需要用 2 个遍历指针，分别进行对应的迭代，来达到最佳的查找遍历性能

    *******************************************************************************************************/

    int ary_a[] = { 1,3,5,7,8,9 };
    Node* head_a  = generateLinkTable(ary_a, ( sizeof(ary_a)/ sizeof(ary_a[0]) ) );
    printTb("Original Linked Table A : ", head_a);
    printf("\n");

    int ary_b[] = { 6,8 };
    Node* head_b  = generateLinkTable(ary_b, ( sizeof(ary_b)/ sizeof(ary_b[0]) ) );
    printTb("Original Linked Table B : ", head_b);
    printf("\n");


    Node* aItPre = head_a;
    Node* aIt = head_a->next;
    Node* bIt = head_b->next;
    while ( aIt!= NULL   &&   bIt!=NULL )
    {
        if ( bIt->data < aIt->data )
        {
            bIt = bIt->next;
        }
        else if ( bIt->data == aIt->data )
        {
            // Do Delete Operator
            aItPre->next = aIt->next;

            // delete A
            aIt->next = NULL;
            free(aIt);

            // Both a && b advance 1 step
            aIt = aItPre->next;
            bIt = bIt->next;
        }
        else // bIt->data > aIt->data
        {
            aIt = aIt->next;
            aItPre = aItPre->next;
        }
    }

    printTb("C = A - B  : ", head_a);
    printf("\n");
    
    freeTable(head_a);  head_a = NULL;
    freeTable(head_b);  head_b = NULL;
    
}

int main(int argc, char* argv[])
{
    Subscract2LinkTableWithRandomOrder();
    printf("\n\n");

    SubstractLinkTableWithAscendingOrder();
    printf("\n\n");
    
    return 0;
}


