#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using i64 = long long int;

const size_t SHORT_JUMP = 32;
const size_t MED_JUMP = 512;
const size_t LONG_JUMP = 16384;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()

struct EdgeInfo {
  EdgeInfo(size_t t) : target(t) {}
  size_t target;
  int num_nodes = 0;
  i64 total_distance = 0;
};

struct Node {
  std::vector<EdgeInfo> edges;
  size_t depth = 0;
  size_t up_edge = 0;
  size_t up_index = 0;
  size_t shortjump = 0;
  size_t medjump = 0;
  size_t longjump = 0;
  i64 total_distance_below = 0;

  void add_edge(size_t o) {
    edges.emplace_back(o);
  }
};

struct Nodes {
  Nodes(size_t N) : nodes(N), root(128'189 % N) {}

  void add(size_t u, size_t v) {
    nodes[u].add_edge(v);
    nodes[v].add_edge(u);
  }

  std::pair<int, i64> get_edge_info(size_t s, size_t t, size_t depth, size_t shortjump, size_t medjump, size_t longjump) {
    auto& node = nodes[t];
    node.depth = depth;
    node.up_edge = -1;
    node.up_index = s;
    node.shortjump = shortjump;
    node.medjump = medjump;
    node.longjump = longjump;

    size_t next_shortjump = depth % SHORT_JUMP == 0 ? t : shortjump;
    size_t next_medjump = depth % MED_JUMP == 0 ? t : medjump;
    size_t next_longjump = depth % LONG_JUMP == 0 ? t : longjump;

    int num_nodes = 0;
    i64 total_distance = 0;
    std::sort(node.edges.begin(), node.edges.end(), [](const EdgeInfo& l, const EdgeInfo& r) {
        return l.target < r.target;
    });
    for (size_t i = 0; i < node.edges.size(); ++i) {
      auto& edge = node.edges[i];
      if (edge.target != s) {
        const auto [nn, td] = get_edge_info(t, edge.target, depth+1, next_shortjump, next_medjump, next_longjump);
        edge.num_nodes = nn;
        edge.total_distance = td;
        num_nodes += nn;
        total_distance += td;
      } else {
        node.up_edge = i;
      }
    }

    node.total_distance_below = total_distance;

    num_nodes += 1;
    total_distance += num_nodes;
    return std::make_pair(num_nodes, total_distance);
  }

  void put_edge_info(size_t t, int num_nodes_above, i64 total_distance_above) {
    auto& node = nodes[t];

    const int total_nodes = nodes.size();
    const i64 total_distance = node.total_distance_below + total_distance_above;

    for (size_t i = 0; i < node.edges.size(); ++i) {
      auto& edge = node.edges[i];
      if (i == node.up_edge) {
        edge.num_nodes = num_nodes_above;
        edge.total_distance = total_distance_above;
      } else {
        int nodes = total_nodes - edge.num_nodes;
        i64 distance = total_distance - edge.total_distance + nodes;
        put_edge_info(edge.target, nodes, distance);
      }
    }
  }

  void build_info() {
    get_edge_info(-1, root, 0, -1, -1, -1);
    put_edge_info(root, 0, 0);
  }

  i64 query(size_t a, size_t b) const {
    if (nodes[a].depth > nodes[b].depth) std::swap(a, b);
    const size_t a_dp = nodes[a].depth;
    const size_t b_dp = nodes[b].depth;

    const size_t join = [&]() {
      size_t bp = b;
      while (((nodes[bp].depth - 1) & ~(LONG_JUMP - 1)) > a_dp) {
        bp = nodes[bp].longjump;
      }
      while (((nodes[bp].depth - 1) & ~(MED_JUMP - 1)) > a_dp) {
        bp = nodes[bp].medjump;
      }
      while (((nodes[bp].depth - 1) & ~(SHORT_JUMP - 1)) > a_dp) {
        bp = nodes[bp].shortjump;
      }
      while (nodes[bp].depth > a_dp) {
        bp = nodes[bp].up_index;
      }
      size_t ap = a;
      while (ap != bp) {
        if (nodes[ap].longjump != nodes[bp].longjump) {
          ap = nodes[ap].longjump;
          bp = nodes[bp].longjump;
        } else if (nodes[ap].medjump != nodes[bp].medjump) {
          ap = nodes[ap].medjump;
          bp = nodes[bp].medjump;
        } else if (nodes[ap].shortjump != nodes[bp].shortjump) {
          ap = nodes[ap].shortjump;
          bp = nodes[bp].shortjump;
        } else {
          ap = nodes[ap].up_index;
          bp = nodes[bp].up_index;
        }
      }
      return ap;
    }();
    const size_t j_dp = nodes[join].depth;
    const size_t dist = a_dp + b_dp - 2*j_dp;
    const size_t ad = dist / 2;
    const size_t bd = (dist - 1) / 2;

    const size_t b_cut = [&]() {
      size_t bp = b;
      size_t steps_left = bd;
      while (steps_left >= LONG_JUMP) {
        size_t next_bp = nodes[bp].longjump;
        steps_left -= nodes[bp].depth - nodes[next_bp].depth;
        bp = next_bp;
      }
      while (steps_left >= MED_JUMP) {
        size_t next_bp = nodes[bp].medjump;
        steps_left -= nodes[bp].depth - nodes[next_bp].depth;
        bp = next_bp;
      }
      while (steps_left >= SHORT_JUMP) {
        size_t next_bp = nodes[bp].shortjump;
        steps_left -= nodes[bp].depth - nodes[next_bp].depth;
        bp = next_bp;
      }
      while (steps_left > 0) {
        bp = nodes[bp].up_index;
        --steps_left;
      }
      return bp;
    }();

    const size_t a_cut = nodes[b_cut].up_index;
    const auto& be = nodes[b_cut].edges[nodes[b_cut].up_edge];
    const auto& ae = [&]() -> EdgeInfo const& {
      return *std::lower_bound(nodes[a_cut].edges.begin(), nodes[a_cut].edges.end(), b_cut,
          [](const EdgeInfo& e, size_t t) { return e.target < t; });
    }();

    const i64 a_contrib = [&]() {
      i64 total = nodes[a].total_distance_below;
      if (nodes[a].up_edge != size_t(-1)) {
        total += nodes[a].edges[nodes[a].up_edge].total_distance;
      }
      return total;
    }();

    const i64 b_contrib = [&]() {
      return nodes[b].edges[nodes[b].up_edge].total_distance + nodes[b].total_distance_below;
    }();

#if PRINT
    std::cout << "a contrib: " << a_contrib << " dist: " << ad
      << " nn: " << ae.num_nodes << " td: " << ae.total_distance << '\n';
    std::cout << "b contrib: " << b_contrib << " dist: " << bd
      << " nn: " << be.num_nodes << " td: " << be.total_distance << '\n';
#endif

    const i64 a_val = a_contrib - ad * ae.num_nodes - ae.total_distance;
    const i64 b_val = b_contrib - bd * be.num_nodes - be.total_distance;

    return a_val + b_val;
  }

  std::vector<Node> nodes;
  size_t root;

#if PRINT
  void show_all() const {
    for (const auto& node : nodes) {
      std::cout << "Node " << (&node - nodes.data()) << ":\n";
      std::cout << "Depth: " << node.depth;
      if (node.up_edge != size_t(-1)) {
        std::cout << " via " << node.edges[node.up_edge].target;
        std::cout << "\nlong jump: " << node.longjump << " shortjump: " << node.shortjump;
      }
      std::cout << '\n';
      std::cout << "Total distance below: " << node.total_distance_below << '\n';
      for (const auto& edge : node.edges) {
        std::cout << "- Edge " << (&edge - node.edges.data()) << " to " << edge.target << ":\n";
        std::cout << "| Num nodes: " << edge.num_nodes << "\n";
        std::cout << "| Total distance: " << edge.total_distance << "\n";
      }
    }
  }
#endif
};

void go() {
  READ(int, N);
  READ(int, Q);
  Nodes nodes(N);
  for (int i = 0; i < N-1; ++i) {
    READ(int, u);
    READ(int, v);
    nodes.add(u-1, v-1);
  }

  nodes.build_info();
#if PRINT
  nodes.show_all();
  std::cout.flush();
#endif

  for (int i = 0; i < Q; ++i) {
    READ(int, a);
    READ(int, b);
    std::cout << nodes.query(a-1, b-1) << '\n';
#if PRINT
    std::cout.flush();
#endif
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
