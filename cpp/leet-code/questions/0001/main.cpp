/*

1. 两数之和

给定一个整数数组 nums 和一个整数目标值 target，请你在该数组中找出 和为目标值 target  的那 两个 整数，并返回它们的数组下标。

你可以假设每种输入只会对应一个答案，并且你不能使用两次相同的元素。

你可以按任意顺序返回答案。

 

示例 1：

输入：nums = [2,7,11,15], target = 9
输出：[0,1]
解释：因为 nums[0] + nums[1] == 9 ，返回 [0, 1] 。

示例 2：

输入：nums = [3,2,4], target = 6
输出：[1,2]
示例 3：

输入：nums = [3,3], target = 6
输出：[0,1]
 


提示：

-  2 <= nums.length <= 104
-  -109 <= nums[i] <= 109
-  -109 <= target <= 109
-  只会存在一个有效答案
 

进阶：你可以想出一个时间复杂度小于 O(n2) 的算法吗？

*/




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


int main(int argc, char* argv[] )
{
	Solution test;
	auto retArray = test.twoSum(vector<int>{ 1,1,2,2,3,3,4,4 }, 2 );
	cout << "Ret Pair :  [ " << retArray[0] << " , " << retArray[1] << " ] " << endl;

	return 0;
}



