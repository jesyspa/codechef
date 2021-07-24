#include <iostream>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

void go() {
  READ(int, RA);
  READ(int, WA);
  READ(int, CA);
  READ(int, RB);
  READ(int, WB);
  READ(int, CB);

  int n = (RA > RB) + (WA > WB) + (CA > CB); 
  std::cout << (n > 1 ? 'A' : 'B') << '\n';
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
