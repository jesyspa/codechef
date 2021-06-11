#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>

using u32 = unsigned;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()

template<typename T>
struct ShowVecImpl {
  ShowVecImpl(std::vector<T> const& v) : v(&v) {}
  std::vector<T> const* v;
};

template<typename T>
ShowVecImpl<T> show(std::vector<T> const& v) { return ShowVecImpl<T>(v); }

template<typename T>
std::ostream& operator<<(std::ostream& os, ShowVecImpl<T> pvi) {
  auto const& v = *pvi.v;
  os << '{';
  if (!v.empty()) {
    os << v[0];
    for (size_t i = 1; i < v.size(); ++i) {
      os << ", " << v[i];
    }
  }
  os << '}';
  return os;
}

bool is_pow2(u32 x) {
  return (x & (x-1)) == 0;
}

u32 xorsum(u32 n) {
  switch (n & 3) {
    case 0: return n;
    case 1: return 1;
    case 2: return n+1;
    case 3: return 0;
  }
  std::terminate();
}

u32 get_bits(u32 n) {
  u32 i = 0;
  while (n) {
    n >>= 1;
    ++i;
  }
  return i;
}

template<typename F>
struct Solver : F {
  Solver(u32 n, u32 k) : N(n), K(k), 
                         bits(get_bits(N)), mask((1 << bits) - 1),
                         M(xorsum(N)), Mi(~M & mask), L(1 << (bits-1))
                         {}

  using F::add;
  using F::add_range;

  template<typename... T>
  void add_many(T... ts) {
    int arr[] = {(add(ts), 0)...}; (void)arr;
  }

  void add_not_ignored() {
    add_range(0, ignored.front());
    for (size_t i = 1; i < ignored.size(); ++i) {
      add_range(ignored[i-1], ignored[i]);
    }
    add_range(ignored.back(), N+1);
  }

  u32 optimum() const {
    if (K == 1 || (K+2 == N && is_pow2(N+2))) return N;
    if (K+1 == N) return select(mask, N, mask, N, N);
    if (K == N) return M;

    return mask;
  }

  void solve_1() {
    std::cout << __FUNCTION__ << '\n';
    add(N);
  }

  void solve_2() {
    std::cout << __FUNCTION__ << '\n';
    add_many(L-1, L);
  }

  void solve_3() {
    std::cout << __FUNCTION__ << '\n';
    add_many(1, L-2, L);
  }

  void solve_4() {
    std::cout << __FUNCTION__ << '\n';
    add_many(2, 3, L-2, L);
  }

  void solve_5() {
    std::cout << __FUNCTION__ << '\n';
    add_many(3, L-3, L-2, L-1, L);
  }

  void solve_Nmin5() {
    std::cout << __FUNCTION__ << '\n';
    ignored.push_back(select(1, (~N & mask) + 1, 1, L-1, 2));
    ignored.push_back(select(Mi-1, N, Mi+1, L, 3));
    add_not_ignored();
  }

  void solve_Nmin4() {
    std::cout << __FUNCTION__ << '\n';
    ignored.push_back(select(1, (~N & mask) + 1, 1, L-1, 2));
    ignored.push_back(select(Mi-1, N, Mi+1, L, 3));
    add_not_ignored();
  }

  void solve_Nmin3() {
    std::cout << __FUNCTION__ << '\n';
    switch (N & 3) {
      case 0:
        ignored = { Mi-1, 4, 5};
        std::sort(ignored.begin(), ignored.end());
        break;
      case 1:
        ignored = { 3, L-3, L };
        break;
      case 2:
        ignored = { 1, 2, Mi+3};
        break;
      case 3:
        ignored = { 1, L-2, L };
        break;
    }
    add_not_ignored();
  }

  void solve_Nmin2() {
    std::cout << __FUNCTION__ << '\n';
    ignored.push_back(select(1, (~N & mask) + 1, 1, L-1, 2));
    ignored.push_back(select(Mi-1, N, Mi+1, L, 3));
    add_not_ignored();
  }

  void solve_Nmin1() {
    std::cout << __FUNCTION__ << '\n';
    ignored.push_back(select(Mi, N-1, Mi, N, 1));
    add_not_ignored();
  }

  void solve_N() {
    std::cout << __FUNCTION__ << '\n';
    add_range(0, N+1);
  }


  void solve() {
    if (2*K <= N) {
      if (K == 1) return solve_1();
      if (K == 2) return solve_2();
      if (K == 3) return solve_3();
      if (K == 4) return solve_4();
      if (K == 5) return solve_5();
    } else {
      //if (K+5 == N) return solve_Nmin5();
      //if (K+4 == N) return solve_Nmin4();
      if (K+3 == N) return solve_Nmin3();
      if (K+2 == N) return solve_Nmin2();
      if (K+1 == N) return solve_Nmin1();
      if (K == N) return solve_N();
    }
    std::cout << "don't know how to.\n";
  }

  void go() {
    std::cout << "Problem (" << N << ", " << K << "): ";
    solve();

    F::commit(N, K, optimum());
  }

  u32 select(u32 x0, u32 x1, u32 x2, u32 x3, u32 xs) const {
    if (is_pow2(N+2)) return xs;
    switch (N & 3) {
      case 0: return x0;
      case 1: return x1;
      case 2: return x2;
      case 3: return x3;
    }
    std::terminate();
  }

  u32 N, K;
  u32 bits, mask;
  u32 M, Mi, L;

  std::vector<u32> ignored;
};

struct VectorHelper {
  void add(u32 x) {
    result.push_back(x);
  }

  void add_range(u32 x, u32 y) {
    for (size_t i = x+1; i < y; ++i) {
      add(i);
    }
  }

  void commit(u32 N, u32 K, u32 o) {
    if (result.size() != K) { std::cerr << "Wrong number of elements selected: " << show(result) << '\n'; }
    u32 v = 0;
    u32 prev = 0;
    for (auto r : result) {
      if (prev > r) { std::cerr << "Result ordered wrong: " << show(result) << "\n"; }
      if (prev == r) { std::cerr << "Result has duplicate " << r << ".\n"; }
      if (r > N) { std::cerr << "Result contains too large value " << r << ".\n"; }
      if (r == 0) { std::cerr << "Result contains zero.\n"; }
      prev = r;
      v ^= r;
    }
    if (v != o) {
      std::cerr << "Optimum not achieved.  Expected: " << o << " Got: " << v << '\n';
      std::cerr << show(result) << '\n';
    }
  }

  std::vector<u32> result;
};


int main() {
/*
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
*/
  READ(int, T);
  for (int i = 1; i < T; ++i) {
    for (int j = 1; j <= i; ++j) {
      Solver<VectorHelper> solver(i, j);
      solver.go();
    }
  }
}
