#pragma once
#include <string>



typedef struct TreeNode {
    static bool G_TRACE_MEMORY_LEAK;
    static int  G_NODE_CNT;


    std::string val;
    struct TreeNode* left;
    struct TreeNode* right;
    
    TreeNode(const std::string& v) : val(v), left(nullptr), right(nullptr) { 
        if ( G_TRACE_MEMORY_LEAK ) { ++G_NODE_CNT; }
    }
    virtual ~TreeNode() {
        if ( left != nullptr ) {
            delete left; left = nullptr;
        }

        if ( right != nullptr ) {
            delete right; right = nullptr;
        }

        if ( G_TRACE_MEMORY_LEAK ) { --G_NODE_CNT; }
        // delete this;
    }

} Node;



