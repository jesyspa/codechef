#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>

using u64 = long long unsigned int;

constexpr u64 MOD = 1'000'000'007;
constexpr u64 MAX_Q = 100'000;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

template <typename T>
T modpow(T b, T e) {
  if (e == 0) return 1;
  if (e == 1) return b;
  T r = modpow(b, e/2);
  r = (r * r) % MOD;
  if (e % 2 == 1)
    r = (r * b) % MOD;
  return r;
}

u64 to_chefora(u64 x) {
  u64 r = x;
  x /= 10;
  while (x) {
    r *= 10;
    r += x % 10;
    x /= 10;
  }
  return r;
}

struct ChefInfo {
  u64 value, sum;
};

u64 go(std::vector<ChefInfo> const& info) {
  READ(u64, L);
  READ(u64, R);
  u64 base = info[L].value;
  u64 exp = info[R].sum - info[L].sum;
  return modpow(base, exp);
}

int main() {
  std::vector<ChefInfo> info;
  info.reserve(MAX_Q + 1);
  u64 last_sum = 0;
  for (u64 i = 0; i <= MAX_Q; ++i) {
    u64 chefora = to_chefora(i);
    last_sum += chefora;
    info.push_back({chefora, last_sum});
  }
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    std::cout << go(info) << '\n';
  }
}
