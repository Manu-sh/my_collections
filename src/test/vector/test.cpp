#include "../abench/AlgoRank.cpp"

extern "C" {
	#include <unistd.h>
	#include "vector_int.h"
}

#include <cstdio>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

vector<int> get_sample(int size) {

	srand(time(0));
	vector<int> vct(size);

	for (size_t i = 0; i < vct.size(); i++)
		vct[i] = rand();

	return vct;
}

void check_correctness(const vector<int> &sample) {

	vector<int> pvct;
	vector_int *vct = vector_int_new();
	assert(vct);

	for (auto s : sample) {

		vector_int_pushBack(vct, s);
		pvct.push_back(s);

		int top[2];
		top[0] = vector_int_back(vct);
		top[1] = pvct.back();

		assert(top[0] == top[1]);
	}


	assert((int)pvct.size() == vector_int_length(vct));

	while (!vector_int_isEmpty(vct)) {

		int top[2];
		top[0] = vector_int_back(vct);
		top[1] = pvct.back();

		assert(top[0] == top[1]);

		(void)vector_int_popBack(vct);
		pvct.pop_back();
	}

	assert( vector_int_isEmpty(vct) && pvct.empty() );
	vector_int_free(vct);
}

void benchmark_cpp(const vector<int> &sample) {

	vector<int> pvct;

	for (auto s : sample) {
		pvct.push_back(s);
		pvct.back();
	}

	// cout << "pvct: " << pvct.capacity() << endl;

	while (!pvct.empty()) {
		pvct.back();
		pvct.pop_back();
	}

}

void benchmark_c(const vector<int> &sample) {

	vector_int *vct = vector_int_new();

	for (auto s : sample) {
		vector_int_pushBack(vct, s);
		vector_int_back(vct);
	}

	// cout << "vct: " << vector_int_capacity(vct) << endl;

	while (!vector_int_isEmpty(vct)) {
		vector_int_back(vct);
		vector_int_popBack(vct);
	}

	vector_int_free(vct);

}

int main() {

	AlgoRank tab;
	using std::string_literals::operator""s;

	for (auto sample_size : {100,1000,10000,100000,1000000}) {

		const auto &sample = get_sample(sample_size);
		check_correctness(sample);

		const auto &ssample_size = to_string(sample_size);
		vector<string> v{
			"cpp ("s + ssample_size + ")"s,
			"c   ("s + ssample_size + ")"s
		};

		/* tab take his own copy of .c_str() so there aren't problems when vector will be destroyed */
		tab.bench( v[0].c_str(), [&sample] { benchmark_cpp(sample); });
		tab.bench( v[1].c_str(), [&sample] { benchmark_c(sample); });

	}

	cout << tab << endl;

}
