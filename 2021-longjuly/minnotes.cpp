#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <limits>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(a, b) do { a = std::min(a, b); } while(0)

size_t next_binpow(size_t N) {
  if ((N & (N-1)) == 0) return N;
  for (int i = 0; N & ~(1 << i); ++i) {
    N &= ~(1 << i);
  }
  return N << 1;
}

template<typename T>
struct SegmentTree {
  struct Elem {
    size_t low, high;
    T val;
  };

  SegmentTree(size_t N) {
    size_t B = next_binpow(N);
    data_.resize(2*B-1);
    next_free_ = ones_start_ = B - 1;
  }

  void add(T t) {
    data_[next_free_++].val = t;
    sum_ += t;
  }

  T preprocess(size_t i, size_t low, size_t high) {
    data_[i].low = low;
    data_[i].high = high;
    if (i >= ones_start_) return data_[i].val;
    const size_t m = low + (high-low)/2;
    T a = preprocess(2*i+1, low, m);
    T b = preprocess(2*i+2, m, high);
    return data_[i].val = combine(a, b);
  }

  void preprocess() {
    preprocess(0, 0, ones_start_+1);
  }

  static T combine(T a, T b) {
    if (a > b) combine(b, a);
    if (a == 0) return b;
    return combine(b % a, a);
  }

  T compute_excluding(size_t i, size_t e) const {
    if (e < data_[i].low || e >= data_[i].high) return data_[i].val;
    if (e + ones_start_ == i) return 0;
    return combine(compute_excluding(2*i+1, e), compute_excluding(2*i+2, e));
  }

  T compute_excluding(size_t e) const {
    return compute_excluding(0, e);
  }

  T get(size_t i) const {
    return data_[ones_start_ + i].val;
  }

  std::vector<Elem> data_;
  size_t ones_start_;
  size_t next_free_;
  T sum_ = 0;
};

void go() {
  READ(size_t, N);
  SegmentTree<i64> tree(N);
  for (size_t i = 0; i < N; ++i) {
    READ(i64, A);
    tree.add(A);
  }

  if (N == 1) {
    std::cout << 1 << '\n';
    return;
  }

  tree.preprocess();

  i64 m = std::numeric_limits<i64>::max();
  for (size_t i = 0; i < N; ++i) {
    MIN(m, (tree.sum_ - tree.get(i)) / tree.compute_excluding(i));
  }

  std::cout << (m+1) << '\n';
}

int main() {
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    go();
  }
}
