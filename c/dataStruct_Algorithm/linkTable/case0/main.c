#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    char element;
    struct Node* next;
} Node ;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
Node* utilCreateNode(char content)
{
    Node* nd = (Node*)malloc( sizeof(Node) );
    nd->element = content;
    nd->next = NULL;

    return nd;
}


void utilReleaseTable(Node* head)
{
    Node* current = head;
    Node* next = NULL;

    do {
        next = current->next;
        current->next = NULL;

        free(current);
        current = next;
    } while ( current != NULL );

}

void utilPrintTable(Node* head)
{
    if ( head == NULL ) {
        printf("[ERROR] : Head Node of Link-Table is NULL\n");
        return;
    }

    Node* current = head->next;
    if ( current == NULL ) {
        printf("[INFO] : Link-Table is Empty\n");
        return;
    } 
    // else
    do {
        printf("%c", current->element);
        current = current->next;
        if ( current == NULL ) {
            break;
        } else {
            printf(" -> ");
        }
    } while(1);

    printf("\n");
}

void utilReverseTable(Node* head)
{
    if ( head == NULL ) {
        return;
    }

    Node* current = head->next;
    Node* next    = NULL;
    head->next = NULL;

    while ( current != NULL )
    {
        next = current->next;
        current->next = head->next;
        head->next = current;

        current = next;
    }

}


//
////////////////////////////////////////////////////////////////////////////////////////////////////


Node* createList0()
{
    Node* head = utilCreateNode('?');
    return head;
}


Node* createList1()
{
    Node* head  = utilCreateNode('?');
    Node* node1 = utilCreateNode('A');

    // link 2 nodes
    head->next = node1;
    node1->next = NULL;

    return head;
}


Node* createList2()
{
    Node* head  = utilCreateNode('?');
    Node* node1 = utilCreateNode('A');
    Node* node2 = utilCreateNode('B');

    // link 3 nodes
    head->next  = node1;
    node1->next = node2;
    node2->next = NULL;

    return head;
}

Node* createList3()
{
    Node* head  = utilCreateNode('?');
    Node* node1 = utilCreateNode('A');
    Node* node2 = utilCreateNode('B');
    Node* node3 = utilCreateNode('C');

    // link 3 nodes
    head->next  = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = NULL;

    return head;
}

Node* createList4()
{
    Node* head  = utilCreateNode('?');
    Node* node1 = utilCreateNode('A');
    Node* node2 = utilCreateNode('B');
    Node* node3 = utilCreateNode('C');
    Node* node4 = utilCreateNode('D');

    // link 4 nodes
    head->next  = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = NULL;

    return head;
}


void testCase()
{
    Node* table0 = createList0();
    Node* table1 = createList1();
    Node* table2 = createList2();
    Node* table3 = createList3();
    Node* table4 = createList4();
    
    utilPrintTable(table0);
    utilReverseTable(table0);
    utilPrintTable(table0);
    printf("\n");

    utilPrintTable(table1);
    utilReverseTable(table1);
    utilPrintTable(table1);
    printf("\n");

    utilPrintTable(table2);
    utilReverseTable(table2);
    utilPrintTable(table2);
    printf("\n");

    utilPrintTable(table3);
    utilReverseTable(table3);
    utilPrintTable(table3);
    printf("\n");

    utilPrintTable(table4);
    utilReverseTable(table4);
    utilPrintTable(table4);
    printf("\n");

    utilReleaseTable(table0);
    utilReleaseTable(table1);
    utilReleaseTable(table2);
    utilReleaseTable(table3);
    utilReleaseTable(table4);
}



int main(int argc, char* argv[], char* env[])
{
    testCase();
    return 0;
}
