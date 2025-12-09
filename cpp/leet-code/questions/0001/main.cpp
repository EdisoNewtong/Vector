#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;






class Solution {
public:
	/****************************************************************************************************
		 这里先举个比较简单一点的例子 : 
		 有一个一维数组 [ 1, 2, 3, 4, 5, 6 ]  

		 目标数字为  3 :
		 你可以用肉眼就能找到，数组 第0 与第1个 元素之和，就是题目要找的 一对数 
		 那么在从0下标开始遍历数组的过程中，为什么会，当遍历到2时，就已经找到了呢
		 因为 1这个曾经遍历过的数字，被通过了某种数据结构，给记录了下来，当遍历到2时，发现目标数字为3，而1这个数组在之前已经遍历过了。
		 因此，就能得到了 满足条件的  这一对数组了。


		 目标数字为 7 :
		 会有3对满足条件的数 [1,6], [2,5], [3,4]  
										   ^^^^^
										   而第3对，就是能找到的最早的那一对 满足条件的数

		 因此，需要用空间换时间，O(N^2) 的时间复杂度 >  O(N)

		 //
		 // O(N^2)
		 //
		 vector<int> twoSum(vector<int>& nums, int target)
		 {
			  for ( int i = 0; i < SZ-1; ++i )
			  {
				  for ( int j = i+1; i < SZ; ++j )
				  {
					   if ( num[i] + num[j] == target ) {
						   return vector<int>{ i, j };
					   }
				  }
			  }

			  return vector<int>{ -1, -1 };
		 }


		 因此，需要记录之前访问过的元素的值，与它所在的索引位置
		 
		 HashMap<element_value, value_index> 就是一个很好的数据结构
	  -----------------------------------------------------------------------------------------------
		 Notes : 这种结构，可以使 此题的算法时间复杂度， 降低为 O(N) ， 但是空间复杂度 变为 O(N) 
	  -----------------------------------------------------------------------------------------------

	*****************************************************************************************************/

     vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int,int> record;
        vector<int> retPair;
        int SZ = static_cast<int>( nums.size() );
        for ( int i = 0; i < SZ; ++i ) {
            int needed = target - nums[i];
            auto it = record.find(needed);
            if ( it == record.end() ) {
                record.insert( make_pair(nums[i], i ) );
            } else {
                retPair.push_back(i);
                retPair.push_back( it->second );
                return retPair;

            }
        }

        return retPair;
    }

};


int main(int argc, char* argv[] )
{
	Solution test;
	auto retArray = test.twoSum(vector<int>{ 1,1,2,2,3,3,4,4 }, 2 );
	cout << "Ret Pair :  [ " << retArray[0] << " , " << retArray[1] << " ] " << endl;

	return 0;
}




