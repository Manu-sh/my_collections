#include <iostream>
#include <string>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <map>
#include <vector>

#include "Timer.hpp"

class AlgoRank {

	public:
		void bench(const char *fname, std::function<void()> f);
		void add_bench(const char *fname, std::function<double()> f);
		friend std::ostream & operator<<(std::ostream &os, AlgoRank &o);

	private:
		std::unordered_map<std::string, std::pair<double,double>> table;
		bool changed {}; // the default value is false: this avoid 
				 // N/0.0 which is infinte or NaN if N is 0.0
};

void AlgoRank::bench(const char *fname, std::function<void()> f) {

	My::Timer timer;
	f();

	std::get<0>(table[fname]) += timer.elapsed();
	std::get<1>(table[fname]) += 1.0;
	changed = true;
}

void AlgoRank::add_bench(const char *fname, std::function<double()> f) {
	std::get<0>(table[fname]) += f();
	std::get<1>(table[fname]) += 1.0;
	changed = true;
}

std::ostream & operator<<(std::ostream &os, AlgoRank &o) {

	// TODO define an overload: https://stackoverflow.com/questions/18074799/is-stdgreaterdouble-and-stdlessdouble-safe-to-use

	static std::multimap<double, std::string, std::less<double>> results;

	if (o.changed) {
		for (const auto &p : o.table)
			results.insert({ 
				(std::get<0>(p.second) / std::get<1>(p.second)), p.first
			});


		o.changed = false;
	}

	for (const auto &p : results) {
		os << p.second;
		os << std::string(": ");
		os << std::to_string(p.first);
		os << std::string("\n");
	}

	return os;
}

#if 0
using namespace std;
int main() {

	/* 
	   Tab tab{};
	   tab.bench("fname", []{ f(data, i, 0); if (*data != *expected) oshit(); } );
	*/

	AlgoRank tab;

	for (int i = 0; i < 1000000; i++) {
		tab.bench("cout", []{ cout << "hello world" << endl; });
		tab.bench("cerr", []{ cerr << "hello world" << endl; });
	}

	cout << tab;

	return 0;
}
#endif
