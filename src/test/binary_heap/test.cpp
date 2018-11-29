#include "../abench/AlgoRank.cpp"

extern "C" {
	#include <unistd.h>
	#include "priority_queue_int.h"
}

#include <cstdio>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <queue>

using namespace std;

vector<int> get_sample(int size) {

	srand(time(0));
	vector<int> vct(size);

	for (size_t i = 0; i < vct.size(); i++)
		vct[i] = rand();

	return vct;
}

void check_correctness(const vector<int> &sample) {

	bool max_heap = true;

jmp:
	/* test max_heap & min_heap */
	bool (*lt)(int,int) = max_heap ? [] (int a, int b) { return a < b; } : [] (int a, int b) { return b > a; };

	priority_queue<int, vector<int>, decltype(lt)> pqueue(lt);
	priority_queue_int *hp = priority_queue_int_new(lt);
	assert(hp);

	for (auto s : sample) {

		priority_queue_int_add(hp, s);
		pqueue.push(s);

		int top[2];
		top[0] = priority_queue_int_peek(hp);
		top[1] = pqueue.top();

		assert(top[0] == top[1]);
	}


	assert((int)pqueue.size() == priority_queue_int_length(hp));

	while (!priority_queue_int_isEmpty(hp)) {

		int top[2];
		top[0] = priority_queue_int_peek(hp);
		top[1] = pqueue.top();

		assert(top[0] == top[1]);

		(void)priority_queue_int_remove(hp);
		pqueue.pop();

	}

	assert( priority_queue_int_isEmpty(hp) && pqueue.empty() );
	priority_queue_int_free(hp);

	printf("%s [OK]\n", max_heap ? "max heap" : "min heap");

	if (max_heap) {
		max_heap = !max_heap; /* now test min_heap */
		goto jmp;
	}

	// printf("capacity: %d\n", priority_queue_int_capacity(hp));

}

void benchmark_cpp(const vector<int> &sample, bool max_heap) {

	bool (*lt)(int,int) = max_heap ? [] (int a, int b) { return a < b; } : [] (int a, int b) { return b > a; };
	priority_queue<int, vector<int>, decltype(lt)> pqueue(lt);

	for (auto s : sample) {
		pqueue.push(s);
		pqueue.top();
	}

	while (!pqueue.empty()) {
		pqueue.top();
		pqueue.pop();
	}

}

void benchmark_c(const vector<int> &sample, bool max_heap) {

	bool (*lt)(int,int) = max_heap ? [] (int a, int b) { return a < b; } : [] (int a, int b) { return b > a; };
	priority_queue_int *hp = priority_queue_int_new(lt);

	for (auto s : sample) {
		priority_queue_int_add(hp, s);
		priority_queue_int_peek(hp);
	}

	while (!priority_queue_int_isEmpty(hp)) {
		priority_queue_int_peek(hp);
		priority_queue_int_remove(hp);
	}

	priority_queue_int_free(hp);

}

int main() {

	AlgoRank tab;
	using std::string_literals::operator""s;

	for (auto sample_size : {100,1000,10000,100000,1000000}) {

		const bool MAX_HEAP = true, MIN_HEAP = false;
		const auto &sample = get_sample(sample_size);
		check_correctness(sample);

		const auto &ssample_size = to_string(sample_size);
		vector<string> v{
			"cpp max_heap ("s + ssample_size + ")"s,
			"cpp min_heap ("s + ssample_size + ")"s,
			"c max_heap   ("s + ssample_size + ")"s,
			"c min_heap   ("s + ssample_size + ")"s
		};

		/* tab take his own copy of .c_str() so there aren't problems when vector will be destroyed */
		tab.bench( v[0].c_str(), [&sample, MAX_HEAP] { benchmark_cpp(sample, MAX_HEAP); });
		tab.bench( v[1].c_str(), [&sample, MIN_HEAP] { benchmark_cpp(sample, MIN_HEAP); });
		tab.bench( v[2].c_str(), [&sample, MAX_HEAP] { benchmark_c(sample, MAX_HEAP); });
		tab.bench( v[3].c_str(), [&sample, MIN_HEAP] { benchmark_c(sample, MIN_HEAP); });


	}

	cout << tab << endl;

}
