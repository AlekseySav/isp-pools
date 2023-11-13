#include "pool.h"
#include <bits/chrono.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <cassert>
#include <vector>

#define N_POOLS  12'000'000
#define N_CANALS  9'000'000
#define N_WATER  35'000'000
#define MAX_WATER_CHUNK 100

inline unsigned fast_rand(int max) {
	// https://stackoverflow.com/questions/26237419/faster-than-rand
	static unsigned seed = 1;
    seed = (214013 * seed + 2531011);
    return ((seed >> 16) & 0x7fff) % max;
}

void perfomance_test() {
	std::vector<Pool> pools(N_POOLS);
	using cap_t = Pool::cap_t;
	for (cap_t i = 0; i < N_WATER; i++) {
		pools[fast_rand(N_POOLS)].add(fast_rand(MAX_WATER_CHUNK));
	}
	for (cap_t i = 0; i < N_CANALS; i++) {
		pools[fast_rand(N_POOLS)].connect(pools[fast_rand(N_POOLS)]);
	}
	for (cap_t i = 0; i < N_POOLS; i++) {
		(void)pools[i].measure();
	}
	for (cap_t i = 0; i < N_WATER; i++) {
		pools[fast_rand(N_POOLS)].add(fast_rand(MAX_WATER_CHUNK));
	}
	for (cap_t i = 0; i < N_POOLS; i++) {
		(void)pools[i].measure();
	}
}

void basic_test() {
	Pool A, B, C, D;
	assert(A.measure() == 0);
	assert(B.measure() == 0);
	A.add(10);
	B.add(20);
	assert(A.measure() == 10);
	assert(B.measure() == 20);
	A.connect(B);
	assert(A.measure() == 15);
	assert(B.measure() == 15);
	C.connect(A);
	assert(A.measure() == 10);
	assert(B.measure() == 10);
	assert(C.measure() == 10);
	B.add(30);
	assert(A.measure() == 20);
	assert(B.measure() == 20);
	assert(C.measure() == 20);
	std::cerr << "basic tests ok\n";
}

struct PoolCapacity {
	int group;
	Pool::cap_t capacity;
	Pool::cap_t count;
};

void random_test(int n_pools = 1000, int n_operations = 1'000'000) {
	std::cerr << "running stress tests [pools=" << n_pools << ", actions=" << n_operations << "]... ";
	std::vector<Pool> pools(n_pools);
	// for each pool we store manualy calculated capacity
	// when pools are connected, we mark them as linked by setting same group id
	std::vector<PoolCapacity> poolmap(n_pools);
	for (int i = 0; i < n_pools; i++) {
		poolmap[i].count = 1;
		poolmap[i].capacity = 0;
		poolmap[i].group = i;
	}
	for (int i = 0; i < n_operations; i++) {
		int a = fast_rand(n_pools);
		int b = fast_rand(n_pools);
		switch (fast_rand(3)) {
		case 0:
			for (int i = 0; i < n_pools; i++) {
				assert(pools[i].measure() == poolmap[i].capacity / poolmap[i].count);
			}
			break;
		case 1:
			pools[a].add(b);
			for (int j = 0; j < n_pools; j++) {
				if (poolmap[j].group != poolmap[a].group) continue;
				poolmap[j].capacity += b;
			}
			break;
		case 2:
			pools[a].connect(pools[b]);
			if (poolmap[a].group != poolmap[b].group) {
				int old = poolmap[b].group;
				Pool::cap_t count = poolmap[a].count + poolmap[b].count;
				Pool::cap_t cap = poolmap[a].capacity + poolmap[b].capacity;
				for (int j = 0; j < n_pools; j++) {
					if (poolmap[j].group == old) poolmap[j].group = poolmap[a].group;
					if (poolmap[j].group != poolmap[a].group) continue;
					poolmap[j].count = count;
					poolmap[j].capacity = cap;
				}
			}
			break;
		}
	}
	std::cerr << "ok\n";
}

auto timeit(void (*f)()) {
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	f();
	auto stop = high_resolution_clock::now();
	return duration_cast<milliseconds>(stop - start).count();
}

int main() {
	basic_test();
	random_test(5, 1'000);
	random_test(5, 1'000'000);
	random_test(1'000, 1'000'000);
	random_test(5'000, 1'000'000);
	std::cerr << "--- running performance test [N_POOLS = 12M, N_CANALS = 9M, N_WATER = 35M] ---\n";
	std::cerr << "performance test executed in " << timeit(perfomance_test) << " ms\n";
}
