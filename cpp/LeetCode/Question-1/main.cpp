#include <iostream>
#include <vector>
using namespace std;



class Solution
{
public:
	vector<int> twoSum(vector<int>& nums, int target) {
		vector<int> retPair;
		return retPair;
	}
};



int main(int argc, char* argv[])
{
	vector<int> array;
	int target = 0;

	Solution s;
	auto retAry = s.twoSum(array, target);
	int idx = 1;
	for ( const auto& e : retAry )
	{
		cout << idx++ << ". " << e << endl;
	}


	return 0;
}
