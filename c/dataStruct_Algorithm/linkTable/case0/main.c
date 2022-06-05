#include <stdio.h>
#include <stdlib.h> // for function malloc(...) / free(...)

static int G_cnt = 0;
// bool is included inside stdbool.h
static int G_TraceMemoryChange = 1;

typedef struct Node
{
    char element;
    struct Node* next;
} Node ;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
Node* createNode(char content)
{
    Node* nd = (Node*)malloc( sizeof(Node) );
    if( G_TraceMemoryChange ) {
        printf("Create G_cnt = %d\n", ++G_cnt);
    }
    nd->element = content;
    nd->next = NULL;

    return nd;
}


void releaseTable(Node* head)
{
    Node* current = head;
    while ( current != NULL )
    {
        Node* next = current->next;

        free(current);
        if( G_TraceMemoryChange ) {
            printf("Delete G_cnt = %d\n", --G_cnt);
        }

        current = next;
    }
}

void printTableWithHead(const char* title, Node* head)     // void printTableNoneHead(const char* title, Node* head)  // diff line-1
{                                                          // {
    printf("%s : ", title);                                //     printf("%s : ", title);
    if ( head == NULL || head->next == NULL ) {            //     if ( head == NULL ) {                // diff line-2
        printf("Empty Link-List\n");                       //         printf("Empty Link-List\n");
        return;                                            //         return;
    }                                                      //     }
                                                           //
    // else                                                //     // else
    Node* current = head->next;                            //     Node* current = head;                // diff line-3
    do {                                                   //     do {
        printf("%c", current->element);                    //         printf("%c", current->element);
        current = current->next;                           //         current = current->next;
        if ( current != NULL ) {                           //         if ( current != NULL ) {
            printf(" -> ");                                //             printf(" -> ");
        }                                                  //         }
    } while( current != NULL );                            //     } while( current != NULL );
                                                           //
    printf("\n");                                          //     printf("\n");
}                                                          // }

void printTableNoneHead(const char* title, Node* head)
{
    printf("%s : ", title);
    if ( head == NULL ) {
        printf("Empty Link-List\n");
        return;
    }

    // else
    Node* current = head;
    do {
        printf("%c", current->element);
        current = current->next;
        if ( current != NULL ) {
            printf(" -> ");
        } 
    } while( current != NULL );

    printf("\n");
}


void reverseTableWithHead(Node* head)
{
    if ( head == NULL ) {
        return;
    }

    Node* current = head->next;
    head->next = NULL;

    while ( current != NULL )
    {
        Node* next = current->next;
        current->next = head->next;
        head->next = current;

        current = next;
    }
}


Node* reverseTableWithoutHead(Node* head)
{
    if ( head == NULL ) {
        return head;
    }

    Node* current = head;
    head = NULL;

    while ( current != NULL )
    {
        Node* next = current->next;
        current->next = head;
        head = current;

        current = next;
    }

    return head;
}



//
////////////////////////////////////////////////////////////////////////////////////////////////////


void testCaseHead()
{
    // <Empty> element
    Node* head0 = createNode('?');
    printTableWithHead("original-list", head0);
    reverseTableWithHead(head0);
    printTableWithHead(" reverse-list",head0);
    releaseTable(head0);
    printf("==================================================\n");

    Node* head1 = createNode('?');
    Node* node1_1 = createNode('A');
    head1->next = node1_1;

    printTableWithHead("original-list",head1);
    reverseTableWithHead(head1);
    printTableWithHead(" reverse-list", head1);
    releaseTable(head1);
    printf("==================================================\n");
    

    Node* head2   = createNode('?');
    Node* node2_1 = createNode('A');
    head2->next = node2_1;
    Node* node2_2 = createNode('B');
    node2_1->next = node2_2;

    printTableWithHead("original-list",head2);
    reverseTableWithHead(head2);
    printTableWithHead(" reverse-list",head2);
    releaseTable(head2);
    printf("==================================================\n");

    Node* head3   = createNode('?');
    Node* node3_1 = createNode('A');
    head3->next = node3_1;
    Node* node3_2 = createNode('B');
    node3_1->next = node3_2;
    Node* node3_3 = createNode('C');
    node3_2->next = node3_3;

    printTableWithHead("original-list",head3);
    reverseTableWithHead(head3);
    printTableWithHead(" reverse-list",head3);
    releaseTable(head3);
    printf("==================================================\n");

    Node* head4   = createNode('?');
    Node* node4_1 = createNode('A');
    head4->next = node4_1;
    Node* node4_2 = createNode('B');
    node4_1->next = node4_2;
    Node* node4_3 = createNode('C');
    node4_2->next = node4_3;
    Node* node4_4 = createNode('D');
    node4_3->next = node4_4;

    printTableWithHead("original-list",head4);
    reverseTableWithHead(head4);
    printTableWithHead(" reverse-list",head4);
    releaseTable(head4);
    printf("==================================================\n");
}




void testCaseNoneHead()
{
    // <Empty> element
    Node* head0 = NULL;
    printTableNoneHead("original-list", head0);
    head0 = reverseTableWithoutHead(head0);
    printTableNoneHead(" reverse-list",head0);
    releaseTable(head0);
    printf("==================================================\n");

    Node* head1 = createNode('A');

    printTableNoneHead("original-list",head1);
    head1 = reverseTableWithoutHead(head1);
    printTableNoneHead(" reverse-list", head1);
    releaseTable(head1);
    printf("==================================================\n");
    

    Node* head2   = createNode('A');
    Node* node2_1 = createNode('B');
    head2->next = node2_1;

    printTableNoneHead("original-list",head2);
    head2 = reverseTableWithoutHead(head2);
    printTableNoneHead(" reverse-list",head2);
    releaseTable(head2);
    printf("==================================================\n");

    Node* head3   = createNode('A');
    Node* node3_1 = createNode('B');
    head3->next = node3_1;
    Node* node3_2 = createNode('C');
    node3_1->next = node3_2;

    printTableNoneHead("original-list",head3);
    head3 = reverseTableWithoutHead(head3);
    printTableNoneHead(" reverse-list",head3);
    releaseTable(head3);
    printf("==================================================\n");

    Node* head4   = createNode('A');
    Node* node4_1 = createNode('B');
    head4->next = node4_1;
    Node* node4_2 = createNode('C');
    node4_1->next = node4_2;
    Node* node4_3 = createNode('D');
    node4_2->next = node4_3;

    printTableNoneHead("original-list",head4);
    head4 = reverseTableWithoutHead(head4);
    printTableNoneHead(" reverse-list",head4);
    releaseTable(head4);
    printf("==================================================\n");
}




int main(int argc, char* argv[], char* env[])
{
    printf("##################################################\n");
    printf("###   Head Link-Table : \n");
    printf("##################################################\n\n");
    testCaseHead();

    printf("\n\n");

    printf("##################################################\n");
    printf("###   None-Head Link-Table : \n");
    printf("##################################################\n\n");
    testCaseNoneHead();

    return 0;
}

