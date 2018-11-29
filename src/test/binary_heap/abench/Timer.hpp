extern "C" {
	#include <time.h> 
	#include <stdlib.h> 
	#include <sys/time.h>
}

namespace My {

	class Timer {

		static inline double nowSec() noexcept {
			struct timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);
			return t.tv_sec + t.tv_nsec * 1e-9;
		}

		public:
			Timer(): start{nowSec()} { }
			double elapsed() { return nowSec() - start; }

		private:
			const double start;

	};

}


/*
static inline double nowSec() noexcept {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 1e-6;
}
*/
