// Copyright 2013 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <sys/time.h>
#include <type_traits>
#include <vector>

#include "gflags/gflags.h"
#include "btree_map.h"
#include "btree_set.h"
#include "btree_test.h"

DEFINE_int32(test_random_seed, 123456789, "Seed for srand()");
DEFINE_int32(benchmark_max_iters, 10000000, "Maximum test iterations");
DEFINE_int32(benchmark_min_iters, 100, "Minimum test iterations");
DEFINE_int32(benchmark_target_seconds, 1,
	     "Attempt to benchmark for this many seconds");

using std::allocator;
using std::less;
using std::map;
using std::max;
using std::min;
using std::multimap;
using std::multiset;
using std::set;
using std::string;
using std::vector;

namespace btree {
namespace {

struct RandGen {
  typedef ptrdiff_t result_type;
  RandGen(result_type seed) {
    srand(seed);
  }
  result_type operator()(result_type l) {
    return rand() % l;
  }
};

struct BenchmarkRun {
  BenchmarkRun(const char *name, void (*func)(int));
  void Run();
  void Stop();
  void Start();
  void Reset();

  BenchmarkRun *next_benchmark;
  const char *benchmark_name;
  void (*benchmark_func)(int);
  int64_t accum_micros;
  int64_t last_started;
};

BenchmarkRun *first_benchmark;
BenchmarkRun *current_benchmark;

int64_t get_micros () {
  timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

BenchmarkRun::BenchmarkRun(const char *name, void (*func)(int))
  : next_benchmark(first_benchmark),
    benchmark_name(name),
    benchmark_func(func),
    accum_micros(0),
    last_started(0) {
  first_benchmark = this;
}

#define BTREE_BENCHMARK(name) \
  BTREE_BENCHMARK2(#name, name, __COUNTER__)
#define BTREE_BENCHMARK2(name, func, counter)	\
  BTREE_BENCHMARK3(name, func, counter)
#define BTREE_BENCHMARK3(name, func, counter)	\
  BenchmarkRun bench ## counter (name, func)

void StopBenchmarkTiming() {
  current_benchmark->Stop();
}

void StartBenchmarkTiming() {
  current_benchmark->Start();
}

void RunBenchmarks() {
  for (BenchmarkRun *bench = first_benchmark; bench; 
       bench = bench->next_benchmark) {
    bench->Run();
  }
}

void BenchmarkRun::Start() {
  assert(!last_started);
  last_started = get_micros();
}

void BenchmarkRun::Stop() {
  if (last_started == 0) {
    return;
  }
  accum_micros += get_micros() - last_started;
  last_started = 0;
}

void BenchmarkRun::Reset() {
  last_started = 0;
  accum_micros = 0;
}

void BenchmarkRun::Run() {
  assert(current_benchmark == NULL);
  current_benchmark = this;
  int iters = FLAGS_benchmark_min_iters;
  for (;;) {
    Reset();
    Start();
    benchmark_func(iters);
    Stop();
    if (accum_micros > FLAGS_benchmark_target_seconds * 1000000 ||
	iters >= FLAGS_benchmark_max_iters) {
      break;
    } else if (accum_micros == 0) {
      iters *= 100;
    } else {
      int64_t target_micros = FLAGS_benchmark_target_seconds * 1000000;
      iters = target_micros * iters / accum_micros;
    }
    iters = min(iters, FLAGS_benchmark_max_iters);
  }
  fprintf(stdout, "%s\t%qu\t%qu\n", 
	  benchmark_name, 
	  accum_micros * 1000 / iters, 
	  iters);
  current_benchmark = NULL;
}

// Used to avoid compiler optimizations for these benchmarks.
template <typename T>
void sink(const T& t0) {
  volatile T t = t0;
}

// Benchmark insertion of values into a container.
template <typename T>
void BM_Insert(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values);
  for (int i = 0; i < values.size(); i++) {
    container.insert(values[i]);
  }

  for (int i = 0; i < n; ) {
    // Remove and re-insert 10% of the keys
    int m = min(n - i, FLAGS_benchmark_values / 10);

    for (int j = i; j < i + m; j++) {
      int x = j % FLAGS_benchmark_values;
      container.erase(key_of_value(values[x]));
    }

    StartBenchmarkTiming();

    for (int j = i; j < i + m; j++) {
      int x = j % FLAGS_benchmark_values;
      container.insert(values[x]);
    }

    StopBenchmarkTiming();

    i += m;
  }
}

// Benchmark lookup of values in a container.
template <typename T>
void BM_Lookup(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values);

  for (int i = 0; i < values.size(); i++) {
    container.insert(values[i]);
  }

  V r = V();

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    int m = i % values.size();
    r = *container.find(key_of_value(values[m]));
  }

  StopBenchmarkTiming();

  sink(r); // Keep compiler from optimizing away r.
}

// Benchmark lookup of values in a full container, meaning that values
// are inserted in-order to take advantage of biased insertion, which
// yields a full tree.
template <typename T>
void BM_FullLookup(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values);
  vector<V> sorted(values);
  sort(sorted.begin(), sorted.end());

  for (int i = 0; i < sorted.size(); i++) {
    container.insert(sorted[i]);
  }

  V r = V();

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    int m = i % values.size();
    r = *container.find(key_of_value(values[m]));
  }

  StopBenchmarkTiming();

  sink(r); // Keep compiler from optimizing away r.
}

// Benchmark deletion of values from a container.
template <typename T>
void BM_Delete(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values);
  for (int i = 0; i < values.size(); i++) {
    container.insert(values[i]);
  }

  for (int i = 0; i < n; ) {
    // Remove and re-insert 10% of the keys
    int m = min(n - i, FLAGS_benchmark_values / 10);

    StartBenchmarkTiming();

    for (int j = i; j < i + m; j++) {
      int x = j % FLAGS_benchmark_values;
      container.erase(key_of_value(values[x]));
    }

    StopBenchmarkTiming();

    for (int j = i; j < i + m; j++) {
      int x = j % FLAGS_benchmark_values;
      container.insert(values[x]);
    }

    i += m;
  }
}

// Benchmark steady-state insert (into first half of range) and remove
// (from second second half of range), treating the container
// approximately like a queue with log-time access for all elements.
// This benchmark does not test the case where insertion and removal
// happen in the same region of the tree.  This benchmark counts two
// value constructors.
template <typename T>
void BM_QueueAddRem(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();
  assert(FLAGS_benchmark_values % 2 == 0);

  T container;

  const int half = FLAGS_benchmark_values / 2;
  vector<int> remove_keys(half);
  vector<int> add_keys(half);

  for (int i = 0; i < half; i++) {
    remove_keys[i] = i;
    add_keys[i] = i;
  }

  RandGen rand(FLAGS_test_random_seed);

  random_shuffle(remove_keys.begin(), remove_keys.end(), rand);
  random_shuffle(add_keys.begin(), add_keys.end(), rand);

  Generator<V> g(FLAGS_benchmark_values + FLAGS_benchmark_max_iters);

  for (int i = 0; i < half; i++) {
    container.insert(g(add_keys[i]));
    container.insert(g(half + remove_keys[i]));
  }

  // There are three parts each of size "half":
  // 1 is being deleted from  [offset - half, offset)
  // 2 is standing            [offset, offset + half)
  // 3 is being inserted into [offset + half, offset + 2 * half)
  int offset = 0;

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    int idx = i % half;

    if (idx == 0) {
      StopBenchmarkTiming();
      random_shuffle(remove_keys.begin(), remove_keys.end(), rand);
      random_shuffle(add_keys.begin(), add_keys.end(), rand);
      offset += half;
      StartBenchmarkTiming();
    }

    int e = container.erase(key_of_value(g(offset - half + remove_keys[idx])));
    assert(e == 1);
    container.insert(g(offset + half + add_keys[idx]));
  }

  StopBenchmarkTiming();
}

// Mixed insertion and deletion in the same range using pre-constructed values.
template <typename T>
void BM_MixedAddRem(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;
  typename KeyOfValue<typename T::key_type, V>::type key_of_value;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();
  assert(FLAGS_benchmark_values % 2 == 0);

  T container;
  RandGen rand(FLAGS_test_random_seed);

  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values * 2);

  // Create two random shuffles
  vector<int> remove_keys(FLAGS_benchmark_values);
  vector<int> add_keys(FLAGS_benchmark_values);

  // Insert the first half of the values (already in random order)
  for (int i = 0; i < FLAGS_benchmark_values; i++) {
    container.insert(values[i]);

    // remove_keys and add_keys will be swapped before each round,
    // therefore fill add_keys here w/ the keys being inserted, so
    // they'll be the first to be removed.
    remove_keys[i] = i + FLAGS_benchmark_values;
    add_keys[i] = i;
  }

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    int idx = i % FLAGS_benchmark_values;

    if (idx == 0) {
      StopBenchmarkTiming();
      remove_keys.swap(add_keys);
      random_shuffle(remove_keys.begin(), remove_keys.end(), rand);
      random_shuffle(add_keys.begin(), add_keys.end(), rand);
      StartBenchmarkTiming();
    }

    int e = container.erase(key_of_value(values[remove_keys[idx]]));
    assert(e == 1);
    container.insert(values[add_keys[idx]]);
  }

  StopBenchmarkTiming();
}

// Insertion at end, removal from the beginning.  This benchmark
// counts two value constructors.
template <typename T>
void BM_Fifo(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  Generator<V> g(FLAGS_benchmark_values + FLAGS_benchmark_max_iters);

  for (int i = 0; i < FLAGS_benchmark_values; i++) {
    container.insert(g(i));
  }

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    container.erase(container.begin());
    container.insert(container.end(), g(i + FLAGS_benchmark_values));
  }

  StopBenchmarkTiming();
}

// Iteration (forward) through the tree
template <typename T>
void BM_FwdIter(int n) {
  typedef typename std::remove_const<typename T::value_type>::type V;

  // Disable timing while we perform some initialization.
  StopBenchmarkTiming();

  T container;
  vector<V> values = GenerateValues<V>(FLAGS_benchmark_values);

  for (int i = 0; i < FLAGS_benchmark_values; i++) {
    container.insert(values[i]);
  }

  typename T::iterator iter;

  V r = V();

  StartBenchmarkTiming();

  for (int i = 0; i < n; i++) {
    int idx = i % FLAGS_benchmark_values;

    if (idx == 0) {
      iter = container.begin();
    }
    r = *iter;
    ++iter;
  }

  StopBenchmarkTiming();

  sink(r); // Keep compiler from optimizing away r.
}

typedef set<int32_t> stl_set_int32;
typedef set<int64_t> stl_set_int64;
typedef set<string> stl_set_string;

typedef map<int32_t, intptr_t> stl_map_int32;
typedef map<int64_t, intptr_t> stl_map_int64;
typedef map<string, intptr_t> stl_map_string;

typedef multiset<int32_t> stl_multiset_int32;
typedef multiset<int64_t> stl_multiset_int64;
typedef multiset<string> stl_multiset_string;

typedef multimap<int32_t, intptr_t> stl_multimap_int32;
typedef multimap<int64_t, intptr_t> stl_multimap_int64;
typedef multimap<string, intptr_t> stl_multimap_string;

#define MY_BENCHMARK_TYPES2(value, name, size)                                \
  typedef btree ## _set<value, less<value>, allocator<value>, size>           \
    btree ## _ ## size ## _set_ ## name;                                      \
  typedef btree ## _map<value, int, less<value>, allocator<value>, size>      \
    btree ## _ ## size ## _map_ ## name;                                      \
  typedef btree ## _multiset<value, less<value>, allocator<value>, size>      \
    btree ## _ ## size ## _multiset_ ## name;                                 \
  typedef btree ## _multimap<value, int, less<value>, allocator<value>, size> \
    btree ## _ ## size ## _multimap_ ## name

#define MY_BENCHMARK_TYPES(value, name)  \
  MY_BENCHMARK_TYPES2(value, name, 128); \
  MY_BENCHMARK_TYPES2(value, name, 160); \
  MY_BENCHMARK_TYPES2(value, name, 192); \
  MY_BENCHMARK_TYPES2(value, name, 224); \
  MY_BENCHMARK_TYPES2(value, name, 256); \
  MY_BENCHMARK_TYPES2(value, name, 288); \
  MY_BENCHMARK_TYPES2(value, name, 320); \
  MY_BENCHMARK_TYPES2(value, name, 352); \
  MY_BENCHMARK_TYPES2(value, name, 384); \
  MY_BENCHMARK_TYPES2(value, name, 416); \
  MY_BENCHMARK_TYPES2(value, name, 448); \
  MY_BENCHMARK_TYPES2(value, name, 480); \
  MY_BENCHMARK_TYPES2(value, name, 512); \
  MY_BENCHMARK_TYPES2(value, name, 1024); \
  MY_BENCHMARK_TYPES2(value, name, 1536); \
  MY_BENCHMARK_TYPES2(value, name, 2048)

MY_BENCHMARK_TYPES(int32_t, int32);
MY_BENCHMARK_TYPES(int64_t, int64);
MY_BENCHMARK_TYPES(string, string);

#define MY_BENCHMARK4(type, name, func)                            \
  void BM_ ## type ## _ ## name(int n) { BM_ ## func <type>(n); }  \
  BTREE_BENCHMARK(BM_ ## type ## _ ## name)

// Define NODESIZE_TESTING when running btree_perf.py.

#ifdef NODESIZE_TESTING
#define MY_BENCHMARK3(tree, type, name, func) \
  MY_BENCHMARK4(tree ## _128_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _160_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _192_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _224_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _256_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _288_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _320_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _352_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _384_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _416_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _448_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _480_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _512_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _1024_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _1536_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _2048_ ## type, name, func)
#else
#define MY_BENCHMARK3(tree, type, name, func) \
  MY_BENCHMARK4(tree ## _256_ ## type, name, func); \
  MY_BENCHMARK4(tree ## _2048_ ## type, name, func)
#endif

#define MY_BENCHMARK2(type, name, func)    \
  MY_BENCHMARK4(stl_ ## type, name, func); \
  MY_BENCHMARK3(btree, type, name, func)

#define MY_BENCHMARK(type)                        \
  MY_BENCHMARK2(type, insert, Insert);            \
  MY_BENCHMARK2(type, lookup, Lookup);            \
  MY_BENCHMARK2(type, fulllookup, FullLookup);    \
  MY_BENCHMARK2(type, delete, Delete);            \
  MY_BENCHMARK2(type, queueaddrem, QueueAddRem);  \
  MY_BENCHMARK2(type, mixedaddrem, MixedAddRem);  \
  MY_BENCHMARK2(type, fifo, Fifo);                \
  MY_BENCHMARK2(type, fwditer, FwdIter)

MY_BENCHMARK(set_int32);
MY_BENCHMARK(map_int32);
MY_BENCHMARK(set_int64);
MY_BENCHMARK(map_int64);
MY_BENCHMARK(set_string);
MY_BENCHMARK(map_string);

MY_BENCHMARK(multiset_int32);
MY_BENCHMARK(multimap_int32);
MY_BENCHMARK(multiset_int64);
MY_BENCHMARK(multimap_int64);
MY_BENCHMARK(multiset_string);
MY_BENCHMARK(multimap_string);

} // namespace
} // namespace btree

int main(int argc, char **argv) {
  btree::RunBenchmarks();
  return 0;
}
