#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

void go() {
  READ(int, N); (void)N;
  READ(int, M);
  READ(int, L);

  std::unordered_map<int, int> to_list;
  for (int i = 0; i < M; ++i) {
    READ(int, K);
    for (int j = 0; j < K; ++j) {
      READ(int, c);
      to_list[c] = i;
    }
  }

  int count = 0;
  int prev = -1;
  for (int i = 0; i < L; ++i) {
    READ(int, c);
    int l = to_list[c];
    if (l != prev) {
      ++count;
      prev = l;
    }
  }

  std::cout << count << '\n';
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
