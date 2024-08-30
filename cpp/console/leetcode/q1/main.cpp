#include <unordered_map>

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int,int> hashNumberIdxPair;

        int sz = static_cast<int>( nums.size() );
        int idx = 0;
        for ( idx = 0; idx < sz; ++idx ) {
            int num = nums[idx];
            if ( idx == 0 ) {
                hashNumberIdxPair.insert( make_pair( num ,idx) );
            } else {
                int wanted = target - num;
                auto it = hashNumberIdxPair.find(wanted);
                if ( it!= hashNumberIdxPair.end()  ) {
                    return vector<int>{ idx, it->second  };
                } else {
                    hashNumberIdxPair.insert( make_pair( num ,idx) );
                }
            }
        }

        return vector<int>{ };
        
    }
};
