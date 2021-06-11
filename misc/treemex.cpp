#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

using i64 = long long int;
constexpr i64 MOD = 1'000'000'007;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) const T x = read<T>()

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

struct Node {
  std::vector<size_t> edges;
  std::map<std::pair<int, int>, i64> possibilities;
  int max = 0;
};

struct Nodes {
  Nodes(size_t N) : nodes(N) {}

  void add(size_t u, size_t v) {
    nodes[u].edges.push_back(v);
    nodes[v].edges.push_back(u);
  }

  int preprocess(size_t from, size_t to) {
    auto& node = nodes[to];
    node.edges.erase(std::remove(node.edges.begin(), node.edges.end(), from), node.edges.end());

    int maximum = 0;
    for (auto const e : node.edges) {
      maximum = std::max(maximum, preprocess(to, e));
    }
    node.max = std::min<int>(maximum, node.edges.size())+1;
    return node.max;
  }

  i64 visit(size_t to, int given, int goal) {
    auto& node = nodes[to];
    if (goal > node.max || given == goal) return 0;
    const auto pr = node.possibilities.emplace(std::make_pair(given, goal), 0);
    const auto b = pr.second;
    auto& v = pr.first->second;

    if (b) { // Need to populate v
      if (node.edges.empty()) {
        if (goal == 0) {
          v = 1;
        } else if (goal == 1 && given == 0) {
          v = 1;
        }
      } else {
        std::vector<int> choices(node.edges.size());
        if (goal == 0) {
          for (auto& c : choices) ++c;
        }

        i64 total = 0;
        bool carry = true;
        do {
          i64 subtotal = 1;
          for (size_t i = 0; i < choices.size(); ++i) {
            subtotal *= visit(node.edges[i], goal, choices[i]);
            if (subtotal == 0) break;
            subtotal %= MOD;
          }

          if (subtotal != 0) {
            bool const valid = [&]() -> bool {
              if (std::find(choices.begin(), choices.end(), goal) != choices.end()) return false;
              for (int i = 0; i < goal; ++i) {
                if (i == given) continue;
                if (std::find(choices.begin(), choices.end(), i) == choices.end()) return false;
              }
              return true;
            }();
            if (valid) {
              total += subtotal;
              total %= MOD;
            }
          }

          carry = true;
          for (size_t i = 0; i < choices.size() && carry; ++i) {
            ++choices[i];
            if (choices[i] == goal) ++choices[i];
            if (choices[i] > nodes[node.edges[i]].max) {
              choices[i] = goal == 0 ? 1 : 0;
            } else {
              carry = false;
            }
          }
        } while (!carry);

        v = total;
      }
    }

    return v;
  }

  i64 solve() {
    int max = preprocess(-1, 0);
    i64 total = 0;
    for (int i = 0; i <= max; ++i) {
      total += visit(0, -1, i);
      total %= MOD;
    }
    return total;
  }

  std::vector<Node> nodes;
};

void go() {
  READ(int, N);
  Nodes nodes(N);
  for (int i = 0; i < N-1; ++i) {
    READ(int, u);
    READ(int, v);
    nodes.add(u-1, v-1);
  }

  i64 const solution = nodes.solve() ;
  std::cout << solution << '\n';
}

int main() {
  READ(int, T);
  for (int i = 0; i < T; ++i) {
    go();
  }
}
