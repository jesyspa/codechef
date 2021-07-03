#include <iostream>

using i64 = long long int;
using u32 = unsigned int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()

void go() {
  int counter[32] = {};
  READ(int, N);
  READ(int, K);
  for (int i = 0; i < N; ++i) {
    READ(u32, A);
    for (int i = 0; i < 32; ++i) {
      counter[i] += (A >> i) & 1;
    }
  }

  int total = 0;
  for (auto c : counter) {
    total += c / K + bool(c % K);
  }
  std::cout << total << '\n';
}

int main() {
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    go();
  }
}
