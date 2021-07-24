#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <utility>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

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
    T val = std::numeric_limits<T>::max();
  };

  SegmentTree(size_t N) {
    size_t B = next_binpow(N);
    data_.resize(2*B-1);
    ones_start_ = B - 1;
  }

  void add(T t) {
    data_[ones_start_ + size_].val = t;
    update(size_, 0);
    ++size_;
  }

  T update(size_t pos, size_t i) {
    auto& e = data_[i];
    if (i >= ones_start_) return e.val;
    const size_t m = e.low + (e.high-e.low)/2;
    T t = update(pos, pos < m ? 2*i+1 : 2*i+2);
    MIN(e.val, t);
    return e.val;
  }

  void preprocess(size_t i, size_t low, size_t high) {
    data_[i].low = low;
    data_[i].high = high;
    if (i >= ones_start_) return;
    const size_t m = low + (high-low)/2;
    preprocess(2*i+1, low, m);
    preprocess(2*i+2, m, high);
  }

  void preprocess() {
    preprocess(0, 0, ones_start_+1);
  }

  T get_range_impl(size_t l, size_t r, size_t i) const {
    auto& node = data_[i];
    if (l <= node.low && node.high <= r) {
      return node.val;
    }
    if (node.high <= l || r <= node.low) {
      return std::numeric_limits<T>::max();
    }
    auto lv = get_range_impl(l, r, 2*i+1);
    auto rv = get_range_impl(l, r, 2*i+2);
    return std::min(lv, rv);
  }

  T get_range(size_t l, size_t r) const {
    return get_range_impl(l, r, 0);
  }

  std::vector<Elem> data_;
  size_t ones_start_;
  size_t size_ = 0;
};


void go() {
  READ(size_t, N);
  READ(size_t, K);

  SegmentTree<int> tree(N);
  tree.preprocess();
  tree.add(0);
  for (size_t i = 1; i <= N; ++i) {
    READ(int, H);
    size_t l = K + 1 >= i ? 0 : (i - K - 1);
    int best = tree.get_range(l, i);
    tree.add(best + H);
  }

  size_t l = K >= N ? 0 : (N - K);
  std::cout << tree.get_range(l, N+1) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    go();
  }
}
