#include <iostream>
#include <queue>

using namespace std;

int main() {

	// be careful not to exceed the stack
	const int nums[] = {
		#include "nums.txt"
		,382919,48329,32189,123901,431,341091,21,2,1,3,4,5,7,8,943, 999999
	};

	priority_queue<int> p;

	for (int i = 0; i < sizeof nums/sizeof(int); i++)
		p.push(nums[i]);

	while(!p.empty()) {
		// printf("[%d]\n", p.top());
		p.pop();
	}

	return 0;

}
