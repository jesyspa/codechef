#include <iostream>
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

struct Node {
  size_t even = 0, odd = 0;
};

Node combine(Node a, Node b) {
  return {a.even + b.even, a.odd + b.odd};
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

  T get_range_impl(size_t l, size_t r, size_t i) const {
    auto& node = data_[i];
    if (l <= node.low && node.high <= r) return node.val;
    if (node.high <= l || r <= node.low) return {};
    auto lv = get_range_impl(l, r, 2*i+1);
    auto rv = get_range_impl(l, r, 2*i+2);
    return combine(lv, rv);
  }

  T get_range(size_t l, size_t r) const {
    return get_range_impl(l, r, 0);
  }

  T get(size_t i) const {
    return data_[ones_start_ + i].val;
  }

  std::vector<Elem> data_;
  size_t ones_start_;
  size_t next_free_;
};


void go() {
  READ(size_t, N);
  READ(size_t, Q);

  SegmentTree<Node> tree(N);
  for (size_t i = 0; i < N; ++i) {
    READ(int, a);
    if (a % 2 == 1) {
      tree.add(Node{0, 1});
    } else {
      tree.add(Node{1, 0});
    }
  }
  tree.preprocess();

  for (size_t i = 0; i < Q; ++i) {
    READ(size_t, l);
    READ(size_t, r);

    const Node n = tree.get_range(l-1, r);

    i64 o = n.odd;
    i64 e = n.even;
    i64 all_even = e*(e-1)*(e-2) / 6;
    i64 two_odd = e*o*(o-1) / 2;
    std::cout << (all_even + two_odd) << '\n';
  }
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
