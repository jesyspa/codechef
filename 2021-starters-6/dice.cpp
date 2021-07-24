#include <iostream>
#include <iomanip>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

char const* result[5] = {
  "0", "0.166666", "0.333333", "0.5", "0.666666"
};

void go() {
  READ(int, X);
  READ(int, Y);

  const int Z = 6 - X - Y;
  std::cout << (Z < 0 ? "0" : result[Z]) << '\n';
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
