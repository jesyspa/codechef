#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <utility>

using i64 = long long int;
using u64 = unsigned long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

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

struct Cache {
  i64 sum = 0;
  u64 hash = 0;
};

std::ostream& operator<<(std::ostream& os, const Cache& cache) {
  return os << "{sum: " << cache.sum << ", hash: " << cache.hash << "}";
}

std::vector<i64> read_array(size_t N) {
  std::vector<i64> xs;
  xs.reserve(N);
  for (size_t i = 0; i < N; ++i) {
    READ(i64, x);
    xs.push_back(x);
  }
  return xs;
}

std::vector<Cache> make_caches(std::vector<i64> const& xs) {
  std::vector<Cache> caches;
  caches.reserve(xs.size()+1);

  Cache cache;
  caches.push_back(cache);
  for (u64 const x : xs) {
    cache.sum += x;
    cache.hash ^= u64(x);
    caches.push_back(cache);
  }

  return caches;
}

struct Subarray {
  Cache cache;
  i64 const* data;
  size_t length;
};

Cache combine(const Cache& lhs, const Cache& rhs) {
  return {rhs.sum - lhs.sum, lhs.hash ^ rhs.hash};
}

std::ostream& operator<<(std::ostream& os, const Subarray& arr) {
  os << "Subarray " << arr.cache << ":";
  for (size_t i = 0; i < arr.length; ++i) {
    os << ' ' << arr.data[i];
  }
  return os;
}

bool operator<(const Subarray& lhs, const Subarray& rhs) {
  if (lhs.cache.sum < rhs.cache.sum) return true;
  if (lhs.cache.sum > rhs.cache.sum) return false;
  if (lhs.cache.hash < rhs.cache.hash) return true;
  if (lhs.cache.hash > rhs.cache.hash) return false;
  if (lhs.length < rhs.length) return true;
  if (lhs.length > rhs.length) return false;
  for (size_t i = 0; i < lhs.length; ++i) {
    if (lhs.data[i] < rhs.data[i]) return true;
    if (lhs.data[i] > rhs.data[i]) return false;
  }
  return false;
}

void go() {
  READ(size_t, N);
  const auto xs = read_array(N);
  const auto caches = make_caches(xs);

  std::map<Subarray, i64> freqs;

#ifdef PRINT
  std::cout << "caches: " << show(caches) << '\n';
#endif

  const auto get_subarray = [&](size_t i, size_t l) {
    return Subarray{combine(caches[i], caches[i+l]), &xs[i], l};
  };

  for (size_t l = 1; l <= N; ++l) {
    for (size_t i = 0; i+l <= N; ++i) {
      ++freqs[get_subarray(i, l)];
    }
  }

  i64 max_score = std::numeric_limits<i64>::min();
  for (auto const& [k, v] : freqs) {
    MAX(max_score, v*k.cache.sum);
#ifdef PRINT
    std::cout << k << " * " << v << " = " << (v * k.cache.sum) << '\n';
#endif
  }


  std::cout << max_score << '\n';
#ifdef PRINT
  std::cout.flush();
#endif
}

int main() {
#ifndef PRINT
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
#endif
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    go();
  }
}
