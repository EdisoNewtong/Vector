#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


typedef struct TreeNode
{
    const char*  content;
    struct TreeNode* firstChild;
    struct TreeNode* nextSibling;
} TreeNode;


static int G_NodeCnt = 0;

static TreeNode* g_A = NULL;
static TreeNode* g_B = NULL;
static TreeNode* g_C = NULL;
static TreeNode* g_D = NULL;
static TreeNode* g_E = NULL;
static TreeNode* g_F = NULL;
static TreeNode* g_G = NULL;
static TreeNode* g_H = NULL;
static TreeNode* g_I = NULL;
static TreeNode* g_J = NULL;
static TreeNode* g_K = NULL;
static TreeNode* g_L = NULL;
static TreeNode* g_M = NULL;
static TreeNode* g_N = NULL;
static TreeNode* g_P = NULL;
static TreeNode* g_Q = NULL;




TreeNode* getNodeByName(const char* content, TreeNode* rootOrCurrent)
{
    if (rootOrCurrent != NULL &&   ( strcmp(rootOrCurrent->content, content) == 0)  ) {
        return rootOrCurrent;
    }

    TreeNode* foundNode = NULL;
    TreeNode* childNode = (rootOrCurrent != NULL) ? rootOrCurrent->firstChild : NULL;
    while (childNode != NULL) {
        TreeNode* retNode = getNodeByName(content, childNode);
        if (retNode != NULL) {
            foundNode = retNode;
            break;
        }
        childNode = childNode->nextSibling;
    }

    return foundNode;
}


int getDepth(TreeNode* targetNode, TreeNode* rootOrParent, int level)
{
    int retLevel = level;
    if (rootOrParent == NULL) {
        return -1;
    }

    if (targetNode != rootOrParent) {
        TreeNode* child = rootOrParent->firstChild;
        int foundFlag = false;
        while (child != NULL)
        {
            int subRet = getDepth(targetNode, child, (level + 1));
            if (subRet > level) {
                foundFlag = true;
                retLevel = subRet;
                break;
            }
            child = child->nextSibling;
        }

        if (!foundFlag) {
            return -1;
        }
    }
    // else
    // targetNode == rootOrParent
    //      retLevel don't changed

    return retLevel;
}

int getHeight(TreeNode* node)
{
    int maxH = 0;
    bool hasChild = false;
    for (TreeNode* child = (node != NULL ? node->firstChild : NULL);
        child != NULL;
        child = child->nextSibling)
    {
        hasChild = true;
        int h = getHeight(child);
        if (h > maxH) {
            maxH = h;
        }
    }

    return hasChild ? (maxH + 1) : 0;
}

TreeNode* buildTree()
{

    TreeNode* rootA = (TreeNode*)malloc( sizeof(TreeNode) ); rootA->content = "A"; rootA->firstChild = NULL; rootA->nextSibling = NULL; g_A = rootA; ++G_NodeCnt;

    TreeNode* ndB   = (TreeNode*)malloc( sizeof(TreeNode) ); ndB->content   = "B"; ndB->firstChild = NULL; ndB->nextSibling = NULL;  g_B = ndB; ++G_NodeCnt; 
    TreeNode* ndC   = (TreeNode*)malloc( sizeof(TreeNode) ); ndC->content   = "C"; ndC->firstChild = NULL; ndC->nextSibling = NULL;  g_C = ndC; ++G_NodeCnt;
    TreeNode* ndD   = (TreeNode*)malloc( sizeof(TreeNode) ); ndD->content   = "D"; ndD->firstChild = NULL; ndD->nextSibling = NULL;  g_D = ndD; ++G_NodeCnt;
    TreeNode* ndE   = (TreeNode*)malloc( sizeof(TreeNode) ); ndE->content   = "E"; ndE->firstChild = NULL; ndE->nextSibling = NULL;  g_E = ndE; ++G_NodeCnt;
    TreeNode* ndF   = (TreeNode*)malloc( sizeof(TreeNode) ); ndF->content   = "F"; ndF->firstChild = NULL; ndF->nextSibling = NULL;  g_F = ndF; ++G_NodeCnt;
    TreeNode* ndG   = (TreeNode*)malloc( sizeof(TreeNode) ); ndG->content   = "G"; ndG->firstChild = NULL; ndG->nextSibling = NULL;  g_G = ndG; ++G_NodeCnt;
    TreeNode* ndH   = (TreeNode*)malloc( sizeof(TreeNode) ); ndH->content   = "H"; ndH->firstChild = NULL; ndH->nextSibling = NULL;  g_H = ndH; ++G_NodeCnt;
    TreeNode* ndI   = (TreeNode*)malloc( sizeof(TreeNode) ); ndI->content   = "I"; ndI->firstChild = NULL; ndI->nextSibling = NULL;  g_I = ndI; ++G_NodeCnt;
    TreeNode* ndJ   = (TreeNode*)malloc( sizeof(TreeNode) ); ndJ->content   = "J"; ndJ->firstChild = NULL; ndJ->nextSibling = NULL;  g_J = ndJ; ++G_NodeCnt;
    TreeNode* ndK   = (TreeNode*)malloc( sizeof(TreeNode) ); ndK->content   = "K"; ndK->firstChild = NULL; ndK->nextSibling = NULL;  g_K = ndK; ++G_NodeCnt;
    TreeNode* ndL   = (TreeNode*)malloc( sizeof(TreeNode) ); ndL->content   = "L"; ndL->firstChild = NULL; ndL->nextSibling = NULL;  g_L = ndL; ++G_NodeCnt;
    TreeNode* ndM   = (TreeNode*)malloc( sizeof(TreeNode) ); ndM->content   = "M"; ndM->firstChild = NULL; ndM->nextSibling = NULL;  g_M = ndM; ++G_NodeCnt;
    TreeNode* ndN   = (TreeNode*)malloc( sizeof(TreeNode) ); ndN->content   = "N"; ndN->firstChild = NULL; ndN->nextSibling = NULL;  g_N = ndN; ++G_NodeCnt;
    // 
    TreeNode* ndP   = (TreeNode*)malloc( sizeof(TreeNode) ); ndP->content   = "P"; ndP->firstChild = NULL; ndP->nextSibling = NULL; g_P = ndP; ++G_NodeCnt;
    TreeNode* ndQ   = (TreeNode*)malloc( sizeof(TreeNode) ); ndQ->content   = "Q"; ndQ->firstChild = NULL; ndQ->nextSibling = NULL; g_Q = ndQ; ++G_NodeCnt;

    rootA->firstChild = ndB;
    ndB->nextSibling = ndC;
    ndC->nextSibling = ndD;

    ndD->firstChild = ndH;
    ndD->nextSibling = ndE;

    ndE->firstChild = ndI;
    ndI->nextSibling = ndJ;
    ndJ->firstChild = ndP;
    ndP->nextSibling = ndQ;

    ndE->nextSibling = ndF;
    ndF->firstChild = ndK;
    ndK->nextSibling = ndL;
    ndL->nextSibling = ndM;

    ndF->nextSibling = ndG;
    ndG->firstChild = ndN;

    return rootA;
}


void DestroyTree(TreeNode* pRoot)
{
    if ( pRoot == NULL ) {
        return;
    }

    TreeNode* child = pRoot->firstChild;
    while ( child != NULL ) 
    {
        TreeNode* backupChild = child->nextSibling;
        DestroyTree(child);
        child = backupChild;
    }

    --G_NodeCnt;
    free(pRoot);
     
}

void test1()
{
    TreeNode* rootNode = buildTree();
    TreeNode* allnodeAry[] = { g_A, g_B, g_C, g_D, g_E, g_F, g_G, g_H, g_I, g_J, g_K, g_L, g_M, g_N, g_P, g_Q };

    const char* nodesContentAry[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", /* "O", */ "P", "Q" };
    for ( int i = 0; i < sizeof(nodesContentAry) / sizeof(nodesContentAry[0]); ++i )
    {
        TreeNode* foundNode = getNodeByName( nodesContentAry[i], g_A);
        if (foundNode != NULL) {
            if (foundNode == allnodeAry[i]) {
                printf("[SUCC] : '%s'  has been found\n", nodesContentAry[i]);
            } else {
                printf("[ERROR] : '%s' address is not equal\n", nodesContentAry[i]);
            }
        } else {
            printf("[ERROR] : '%s' is not found\n", nodesContentAry[i]);
        }
    }
    printf("\n");


    for ( int i = 0; i < sizeof(allnodeAry) / sizeof(allnodeAry[0]); ++i ) {
        TreeNode* pNode = allnodeAry[i];
        if ( pNode != NULL ) {
            int height = getHeight(pNode);
            int depth  = getDepth(pNode, g_A, 0);
            printf("%s.  height = %d, depth = %d\n", pNode->content, height, depth);
        }
    }

    DestroyTree(rootNode);
    rootNode = NULL;

    printf("\n");
    if (G_NodeCnt != 0) {
        printf("Memory Leak : %d time(s)\n", G_NodeCnt);
    } else {
        printf("[INFO] No Memory Leak\n");
    }
}




int main(int argc, char* argv[])
{
    test1();
    return 0;
}
