#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <cstring>

using i64 = long long int;
constexpr size_t UP_EDGES = 16;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) auto const x = read<T>()
#define READ_IX(x) auto const x = read<size_t>() - 1
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

struct Node {
  std::vector<size_t> edges;
  Node* up_edges[UP_EDGES] = {};
  size_t depth = 0;

  [[nodiscard]]
  Node* jump_to(size_t k) {
    return jump(depth - k);
  }

  [[nodiscard]]
  Node* jump(size_t k) {
    if (k == 0) return this;
    size_t const i = ffsl(k)-1;
    return up_edges[i]->jump(k - (1 << i));
  }
};

struct Tree {
  Tree(size_t N) : nodes(N) {}

  void add(size_t u, size_t v) {
    nodes[u].edges.push_back(v);
    nodes[v].edges.push_back(u);
  }

  void pp_impl(size_t ix, size_t parent, size_t depth) {
    auto& node = nodes[ix];
    node.depth = depth;
    if (~parent) {
      Node* up = &nodes[parent];
      for (size_t i = 0; up; ++i) {
        node.up_edges[i] = up;
        up = up->up_edges[i];
      }
    }
    for (size_t const e : node.edges) {
      if (e != parent) pp_impl(e, ix, depth+1);
    }
  }

  void preprocess() {
    pp_impl(0, -1, 0);
  }

  [[nodiscard]]
  bool query(std::vector<size_t> const& ixs) {
    if (ixs.size() <= 2) return true;
    auto const get = [&](size_t i) { return &nodes[ixs[i]]; };
    Node* a = get(0);
    size_t i = 1;
    while (i < ixs.size()) {
      Node* node = get(i);
      a = a->jump_to(node->depth);
      if (a != node) break;
      ++i;
    }
    if (i == ixs.size()) return true;
    Node* b = get(i);
    while (i < ixs.size() && a != b) {
      Node* node = get(i);
      if (a->depth != node->depth) {
        a = a->jump_to(node->depth+1);
        b = b->jump_to(node->depth+1);
        if (a == b) return false;
        a = a->jump(1);
        b = b->jump(1);
      }
      if (a != node && b != node) return false;
      ++i;
    }

    return i == ixs.size();
  }

  std::vector<Node> nodes;
};

void go() {
  READ(size_t, N);
  Tree tree(N);

  for (size_t i = 1; i < N; ++i) {
    READ_IX(u);
    READ_IX(v);
    tree.add(u, v);
  }

  tree.preprocess();

  std::vector<size_t> ixs;

  READ(size_t, Q);
  for (size_t q = 0; q < Q; ++q) {
    READ(size_t, K);
    ixs.reserve(K);
    for (size_t i = 0; i < K; ++i) {
      READ_IX(u);
      ixs.push_back(u);
    }
    std::sort(ixs.begin(), ixs.end(), [&](size_t a, size_t b) {
        return tree.nodes[a].depth > tree.nodes[b].depth;
    });
    bool const answer = tree.query(ixs);
    std::cout << (answer ? "YES" : "NO") << '\n';
    ixs.clear();
  }
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
