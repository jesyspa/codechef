#include <iostream>
#include <optional>
#include <vector>
#include <algorithm>
#include <utility>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

struct DigitPair {
  int front, back;
  size_t contrib = 0;
};

void go() {
  READ(size_t, N);
  READ(size_t, M);
  READ(i64, K);

  // read S
  std::vector<DigitPair> S;
  S.reserve(N/2);
  for (size_t i = 0; i < N/2; ++i) {
    READ(char, c);
    S.push_back(DigitPair{c - '0', -1});
  }

  std::optional<int> mid;
  if (N % 2 == 1) {
    READ(char, c);
    mid = c - '0';
  }

  for (size_t i = 0; i < N/2; ++i) {
    READ(char, c);
    S.rbegin()[i].back = c - '0';
  }
  // done reading


  // read rules
  i64 costs[10][10] = {};
  for (size_t i = 0; i < 10; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      if (i != j) costs[i][j] = 1'000'000'001;
    }
  }

  for (size_t i = 0; i < M; ++i) {
    READ(size_t, X);
    READ(size_t, Y);
    READ(i64, W);
    MIN(costs[X][Y], W);
  }
  // done reading

  // ensure optimal routes
  {
    bool changed;
    do {
      changed = false;
      for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
          for (size_t k = 0; k < 10; ++k) {
            if (i == j || j == k) continue;
            if (costs[i][j] + costs[j][k] < costs[i][k]) {
              changed = true;
              costs[i][k] = costs[i][j] + costs[j][k];
            }
          }
        }
      }
    } while (changed);
  }

  const auto cheapest_contrib = [&](int a, int b) {
    i64 min_contrib = 2'000'000'0003;
    for (size_t i = 0; i < 10; ++i) {
      MIN(min_contrib, costs[a][i] + costs[b][i]);
    }
    return min_contrib;
  };

  // check how much we need to make this a palindrome
  i64 coins_left = K;
  for (auto& pair : S) {
    if (pair.front == pair.back) continue;

    pair.contrib = cheapest_contrib(pair.front, pair.back);
    coins_left -= pair.contrib;
  }

  if (coins_left < 0) {
    std::cout << -1 << '\n';
    return;
  }


  const auto update = [&](DigitPair& pair) {
    coins_left += pair.contrib;
    for (size_t i = 9; i < 10; --i) {
      i64 c = costs[pair.front][i] + costs[pair.back][i];
      if (c <= coins_left) {
        pair.front = i;
        pair.back = i;
        pair.contrib = c;
        coins_left -= c;
        return;
      }
    }
    std::terminate(); // should always find something to map to
  };

  for (auto& pair : S) {
    update(pair);
  }

  if (mid.has_value()) {
    [&]() {
      for (size_t i = 9; i < 10; --i) {
        if (costs[*mid][i] <= coins_left) {
          *mid = i;
          return;
        }
      }
      std::terminate(); // should always find something to map to
    }();
  }

  for (size_t i = 0; i < N/2; ++i) {
    std::cout << S[i].front;
  }

  if (N % 2 == 1) {
    std::cout << *mid;
  }

  for (size_t i = 0; i < N/2; ++i) {
    std::cout << S.rbegin()[i].back;
  }
  std::cout << '\n';
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
