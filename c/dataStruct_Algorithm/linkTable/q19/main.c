#include <stdio.h>
#include <stdlib.h> /* system function malloc(...)  /  free(...) required */

typedef int DataType;


/*
    P73 

Q19 :
      用带头结点的单链表，表示整数集合，完成以下算法并分析时间复杂度  
                         
1) 设计一个算法，求 A 并 B ,  C = A 并 B ,   要求不破坏原有的单链表 A 和 B
2) 假设集合中的元素按递增排列,  设计一个高效算法求两个集体 A和B 的并集运算 , C = A 并 B,  要求不破坏原有的单链表 A 和 B


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


void Union2LinkTableWithRandomOrder()
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

    Node* head_c = (Node*)malloc( sizeof(Node) );
    head_c->data = -9999999;
    Node* cur_c = head_c;

    /* Copy table-A  =>   table-C  */
    Node* cur_a = head_a->next;
    while ( cur_a != NULL )
    {
        Node* copyNode = (Node*)malloc( sizeof(Node) );
        copyNode->data = cur_a->data;
        copyNode->next = NULL;

        cur_c->next = copyNode;
        cur_c = copyNode;

        cur_a = cur_a->next;
    }

    /* Merge table-B  into   table-C  */
    Node* cur_b = head_b->next;
    while ( cur_b != NULL )
    {
        Node* copyNode = (Node*)malloc( sizeof(Node) );
        copyNode->data = cur_b->data;
        copyNode->next = NULL;

        Node* cur_cPre = head_c;
        Node* cur_c = head_c->next;
        while ( cur_c != NULL )
        {
            if ( copyNode->data == cur_c->data ) {
                break;
            }

            cur_cPre = cur_cPre->next;
            cur_c = cur_c->next;
        }

        if ( cur_c == NULL ) {
            cur_cPre->next = copyNode;
        }

        cur_b = cur_b->next;
    }

    printTb("Merge A && B into C : ", head_c);
    printf("\n");

    freeTable(head_a);
    freeTable(head_b);
    freeTable(head_c);
}

void Union2LinkTableWithAscendingOrder()
{

    int ary_a[] = { 1,3,5,7,9 };
    Node* head_a  = generateLinkTable(ary_a, ( sizeof(ary_a)/ sizeof(ary_a[0]) ) );
    printTb("Original Linked Table A : ", head_a);
    printf("\n");

    int ary_b[] = { 2,4,6,8,10 };
    Node* head_b  = generateLinkTable(ary_b, ( sizeof(ary_b)/ sizeof(ary_b[0]) ) );
    printTb("Original Linked Table B : ", head_b);
    printf("\n");

    Node* head_c = (Node*)malloc( sizeof(Node) );
    head_c->data = -9999999;
    Node* cur_c = head_c;

    Node* na = head_a->next;
    Node* nb = head_b->next;
    while ( na!=NULL && nb!=NULL )
    {
        Node* copyNode = (Node*)malloc( sizeof(Node) );
        copyNode->next = NULL;

        if ( na->data < nb->data ) {
            /* na < nb */
            copyNode->data = na->data;
            na = na->next;
        } else if ( na->data == nb->data ) {
            /* na == nb */
            copyNode->data = na->data;
            na = na->next;
            nb = nb->next;
        } else {
            /* na > nb */
            copyNode->data = nb->data;
            nb = nb->next;
        }

        cur_c->next = copyNode;
        cur_c = copyNode;
    }

    /* Neither   na   Or   nb   is NULL ( or Both ) */
    if ( na == NULL )  {
        na = nb;
    }

    while ( na != NULL )
    {
        Node* copyNode = (Node*)malloc( sizeof(Node) );
        copyNode->data = na->data;
        copyNode->next = NULL;

        cur_c->next = copyNode;
        cur_c = copyNode;

        na = na->next;
    }

    printTb("Merge A && B into C : ", head_c);
    printf("\n");

    freeTable(head_a);  head_a = NULL;
    freeTable(head_b);  head_b = NULL;
    freeTable(head_c);  head_c = NULL;
    
}

int main(int argc, char* argv[])
{
    Union2LinkTableWithRandomOrder();
    printf("\n\n");
    Union2LinkTableWithAscendingOrder();
    printf("\n\n");
    
    return 0;
}

