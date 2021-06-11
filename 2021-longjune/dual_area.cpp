#include <iostream>
#include <algorithm>
#include <utility>
#include <limits>

using i64 = long long int;
using pair_vector = std::vector<std::pair<i64, i64>>;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

struct Bar {
  Bar(i64 x, i64 const* ptr) : x(x), ptr(ptr) {}
  i64 x;
  i64 const* ptr;
  size_t count = 1;

  // cumulative values extend to the right inclusive.
  i64 cum_low, cum_high;

  i64 low() const { return ptr[0]; }
  i64 high() const { return ptr[count-1]; }
  i64 cum_dist() const { return cum_high - cum_low; }
};

template<typename F>
struct Helper {
  void populate_bars(const pair_vector& points) {
    ys.reserve(points.size());
    i64 prev = -1;
    for (const auto& p : points) {
      ys.push_back(F::value(p));
      if (F::key(p) == prev) {
        ++bars.back().count;
      } else {
        prev = F::key(p);
        bars.emplace_back(prev, &ys.back());
      }
    }
  }

  void populate_cum_dist() {
    i64 bottom = std::numeric_limits<i64>::max();
    i64 top = std::numeric_limits<i64>::min();
    for (auto rit = bars.rbegin(); rit != bars.rend(); ++rit) {
      auto& bar = *rit;
      MIN(bottom, bar.low());
      MAX(top, bar.high());
      bar.cum_low = bottom;
      bar.cum_high = top;
      /*
      std::cout << "Bar " << bar.x << "(" << bar.count << ")" << ": ["
        << bar.cum_low << ", " << bar.low() << ", " << bar.high() << ", " << bar.cum_high << "]\n";
      */
    }
  }

  i64 solve(pair_vector& points) {
    if (points.size() <= 2) return 0;

    std::sort(points.begin(), points.end(), F::cmp);
    populate_bars(points);

    if (bars.size() <= 2) return 0;

    populate_cum_dist();

    const i64 left_edge = bars.front().x;
    const i64 right_edge = bars.back().x;

    i64 minimum = std::numeric_limits<i64>::max();
    const auto propose = [&](i64 e1, i64 l1, i64 h1, i64 e2, i64 l2, i64 h2) {
      const i64 area = (e1 - left_edge)*(h1-l1) + (right_edge - e2)*(h2-l2);
      /*
      std::cout << "Proposal: " << e1 << ", " << l1 << "--" << h1
          << " : " << e2 << ", " << l2 << "--" << h2
          << " (area: " << area << ")"<< '\n';
          */
      MIN(minimum, area);
    };

    // iterators move in alphabetical order: ht, it, jt.
    auto jt = bars.begin(); 
    auto ht = jt++;
    auto it = jt++;

    // These are up to rit
    i64 left_low = ht->low();
    i64 left_high = ht->high();

    while (jt != bars.end()) {
      // Case A: ht in left, it in right
      propose(ht->x, left_low, left_high, it->x, it->cum_low, it->cum_high);

      const auto propose_mid = [&](i64 l1, i64 h1, i64 l2, i64 h2) {
        MIN(l1, left_low);
        MAX(h1, left_high);
        MIN(l2, jt->cum_low);
        MAX(h2, jt->cum_high);
        propose(it->x, l1, h1, it->x, l2, h2);
      };
      // Case B: joined at it, slanted
      for (size_t i = 1; i < it->count; ++i) {
        // left below right
        propose_mid(it->low(), it->ptr[i-1], it->ptr[i], it->high());
        // right below left
        propose_mid(it->ptr[i], it->high(), it->low(), it->ptr[i-1]);
      }

      // incrementation.
      MIN(left_low, it->low());
      MAX(left_high, it->high());
      ht = it;
      it = jt++;
    }

    // This needs to be done an extra time since we never join it+jt
    propose(ht->x, left_low, left_high, it->x, it->cum_low, it->cum_high);

    return minimum;
  }

  std::vector<Bar> bars;
  std::vector<i64> ys;
};

template<typename T>
struct BaseHelper {
  static bool cmp(std::pair<i64, i64> p, std::pair<i64, i64> q) {
    if (T::key(p) < T::key(q)) return true;
    if (T::key(p) > T::key(q)) return false;
    return T::value(p) < T::value(q);
  }
};

struct XYHelper : BaseHelper<XYHelper> {
  static i64 key(std::pair<i64, i64> p) { return p.first; }
  static i64 value(std::pair<i64, i64> p) { return p.second; }
};

struct YXHelper : BaseHelper<YXHelper> {
  static i64 key(std::pair<i64, i64> p) { return p.second; }
  static i64 value(std::pair<i64, i64> p) { return p.first; }
};

void go() {
  READ(int, N);
  pair_vector pairs;
  pairs.reserve(N);
  for (int i = 0; i < N; ++i) {
    READ(i64, x);
    READ(i64, y);
    pairs.emplace_back(x, y);
  }

  const i64 min_xy = [&]() {
    Helper<XYHelper> helper;
    return helper.solve(pairs);
  }();
  const i64 min_yx = [&]() {
    Helper<YXHelper> helper;
    return helper.solve(pairs);
  }();

  std::cout << std::min(min_xy, min_yx) << std::endl;
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
