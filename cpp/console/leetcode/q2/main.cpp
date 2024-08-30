
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */



class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* head  = l1;

        ListNode* preNode = nullptr;
        bool hasLinked = false;
        int plusFlag = 0;
        do {
            int sum = l1->val + (l2!=nullptr ? l2->val : 0) + plusFlag;
            int bit = sum % 10;
            plusFlag = (sum>=10 ? 1 : 0);

            // update the sum result of list#1's node
            l1->val = bit;

            preNode = l1;
            l1 = l1->next;
            if ( l1 == nullptr ) {
                l1 = l2;
                if ( l1!=nullptr ) {
                    l1 = l2->next;
                    preNode->next = l2->next;
                    // break the chain of list#2 and replace the chain by list#1
                    l2 = nullptr;
                }
            } 

            if ( l2!=nullptr ) {
                l2 = l2->next;
            }
        } while ( l1!=nullptr );


        if ( plusFlag > 0 ) {
            preNode->next = new ListNode( plusFlag );
        }

        return head;
    }
};




