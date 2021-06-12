#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <sstream>

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

struct Selection {
  Selection(u32 n, u32 k, bool inv) : N(n), K(k), result(inv ? xorsum(N) : 0), inverted(inv) {} 

  void add(u32 n) {
    num_selected += 1;
    ranges.emplace(n-1, n);
    result ^= n;
  }

  void add_range(u32 l, u32 h) {
    num_selected += h - l;
    ranges.emplace(l, h);
    result ^= xorsum(l) ^ xorsum(h);
  }

  template<typename... T>
  void add_many(T... ts) { int arr[] = {(add(ts), 0)...}; (void)arr; }

  template<typename F>
  void enumerate(F&& f) const {
    if (inverted) {
      auto it = ranges.begin();
      u32 i = N;
      while (it != ranges.end()) {
        if (i > it->second) {
          f(i--);
        } else {
          i = it->first;
          ++it;
        }
      }
      while (i > 0) f(i--);
    } else {
      for (const auto& [l, h] : ranges) {
        for (u32 i = l+1; i <= h; ++i) f(i);
      }
    }
  }

#ifdef VALIDATE
  std::vector<u32> to_vector() const {
    std::vector<u32> nums;
    enumerate([&](u32 i) { nums.push_back(i); });
    return nums;
  }
#endif

  void print() const {
    bool first = true;
    enumerate([&](u32 i) { 
        if (!first) std::cout << ' ';
        else first = false;
        std::cout << i;
    });
    std::cout << '\n';
  }

  u32 N, K, result, num_selected = 0;
  bool inverted = false;

  // ranges are open-closed.
  std::map<u32, u32, std::greater<u32>> ranges;
};

struct Solver {
  Solver(u32 n, u32 k) : N(n), K(k),
                         bits(get_bits(N)), mask((1 << bits) - 1),
                         M(xorsum(N)), Mi(~M & mask), L(1 << (bits-1)),
                         selection(n, k, 2*k > n+(n>7) && k+5 >= n)
                         {}


  u32 optimum() const {
    if (K == 1 || (K+2 == N && is_pow2(N+2))) return N;
    if (K+1 == N) return select(mask, N, mask, N, N);
    if (K == N) return M;

    return mask;
  }

  void solve_1() {
    selection.add(N);
  }

  void solve_2() {
    selection.add_many(L-1, L);
  }

  void solve_3() {
    selection.add_many(1, L-2, L);
  }

  void solve_4() {
    selection.add_many(2, 3, L-2, L);
  }

  void solve_5() {
    selection.add_many(3, L-3, L-2, L-1, L);
  }

  void solve_6() {
    selection.add_many(1, 2, L-3, L-2, L-1, L);
  }

  void solve_Nmin5() {
    switch (N & 3) {
      case 0:
        if (N == L) {
          selection.add_many(L-1, L-2, L-3, 1, 2);
        } else {
          selection.add_many(N, L, L-2, 2, 3);
        }
        break;
      case 1:
        selection.add_many(L, L-1, L-2, L-3, 2);
        break;
      case 2:
        selection.add_many(N, 1, L, L-3, 2);
        break;
      case 3:
        selection.add_many(L, L-1, 1, 2, 3);
        break;
    }
  }

  void solve_Nmin4() {
    switch (N & 3) {
      case 0:
        if (N == L) {
          selection.add_many(N-1, 1, 2, 3);
        } else {
          selection.add_many(N, L, L-3, 2);
        }
        break;
      case 1:
        selection.add_many(L, L-3, 1, 2);
        break;
      case 2:
        selection.add_many(N, 1, L, L-1);
        break;
      case 3:
        selection.add_many(L, L-3, 3, 1);
        break;
    }
  }

  void solve_Nmin3() {
    switch (N & 3) {
      case 0:
        selection.add_many(Mi-1, 4, 5);
        break;
      case 1:
        selection.add_many(3, L-3, L );
        break;
      case 2:
        selection.add_many(1, 2, Mi+3);
        break;
      case 3:
        selection.add_many(1, L-2, L);
        break;
    }
  }

  void solve_Nmin2() {
    selection.add(select(1, (~N & mask) + 1, 1, L-1, 2));
    selection.add(select(Mi-1, N, Mi+1, L, 3));
  }

  void solve_Nmin1() {
    selection.add(select(Mi, N-1, Mi, N, 1));
  }

  void solve_N() {
  }


  void solve() {
    if (selection.inverted) {
      if (K+5 == N) return solve_Nmin5();
      if (K+4 == N) return solve_Nmin4();
      if (K+3 == N) return solve_Nmin3();
      if (K+2 == N) return solve_Nmin2();
      if (K+1 == N) return solve_Nmin1();
      if (K == N) return solve_N();
    } else {
      if (K == 1) return solve_1();
      if (K == 2) return solve_2();
      size_t steps = (K-3)/4;
      if (steps > 0) {
        if (K < L - 3) {
          selection.add_range(3, 3+steps*4);
        } else {
          selection.add_range(3, L-5);
          steps -= (L-8)/4;
          selection.add_range(L+1, L+1+4*steps);
        }
      }
      switch (K & 3) {
        case 0: return solve_4();
        case 1: return solve_5();
        case 2: return solve_6();
        case 3: return solve_3();
      }
    }
  }

  void go() {
    solve();
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

  Selection selection;
};

#ifdef VALIDATE
void verify(std::vector<u32> result, u32 computed_xor, u32 N, u32 K, u32 o) {
  std::stringstream ss;
  ss << "Problem (" << N << ", " << K << ")\n";
  bool err = false;
  if (result.size() != K) { ss << "Wrong number of elements selected: " << show(result) << '\n'; err=true; }
  u32 v = 0;
  u32 prev = 0;
  std::sort(result.begin(), result.end());
  for (auto r : result) {
    if (prev == r) { ss << "Result has duplicate " << r << ".\n"; err=true; }
    if (r > N) { ss << "Result contains too large value " << r << ".\n"; err=true; }
    if (r == 0) { ss << "Result contains zero.\n"; err=true; }
    prev = r;
    v ^= r;
  }
  if (v != o) {
    ss << "Optimum not achieved.  Expected: " << o << " Got: " << v << '\n';
    err = true;
  }
  if (computed_xor != v) {
    ss << "Computed xor disagrees with reality.  Computed: " << computed_xor << " Got: " << v << '\n';
    err = true;
  }
  if (err) {
    ss << "Selection: " << show(result) << '\n';
    std::cerr << ss.str();
  }
}
#endif

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    READ(int, N);
    READ(int, K);
    Solver solver(N, K);
    solver.go();
    solver.selection.print();
#ifdef VALIDATE
    verify(solver.selection.to_vector(), solver.selection.result, i, j, solver.optimum());
#endif
  }
}
