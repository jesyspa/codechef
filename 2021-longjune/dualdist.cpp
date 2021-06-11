#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using i64 = long long int;

const size_t SHORT_JUMP = 1;
const size_t MED_JUMP = 2;
const size_t LONG_JUMP = 4; // 16384;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()

struct Node;

struct EdgeInfo {
  explicit EdgeInfo(Node* t = nullptr) : target(t) {}
  Node* target;
  int num_nodes = 0;
  i64 total_distance = 0;
};

struct Node {
  std::vector<EdgeInfo> edges_;
  size_t depth_ = 0;
  EdgeInfo up_edge_;
  Node* shortjump_ = 0;
  Node* medjump_ = 0;
  Node* longjump_ = 0;
  i64 total_distance_below_ = 0;

#ifdef PRINT
  size_t label_;
#endif

  void add_edge(Node* o) {
    edges_.emplace_back(o);
  }

  std::pair<int, i64> mk_edge_info(Node* s, size_t depth, Node* shortjump, Node* medjump, Node* longjump) {
    depth_ = depth;
    shortjump_ = shortjump;
    medjump_ = medjump;
    longjump_ = longjump;

    Node* next_shortjump = depth % SHORT_JUMP == 0 ? this : shortjump;
    Node* next_medjump = depth % MED_JUMP == 0 ? this : medjump;
    Node* next_longjump = depth % LONG_JUMP == 0 ? this : longjump;

    int num_nodes = 0;
    i64 total_distance = 0;
    std::sort(edges_.begin(), edges_.end(), [](const EdgeInfo& l, const EdgeInfo& r) {
        return l.target < r.target;
    });
    size_t up_edge_ix = -1;
    for (size_t i = 0; i < edges_.size(); ++i) {
      auto& edge = edges_[i];
      if (edge.target != s) {
        const auto [nn, td] = edge.target->mk_edge_info(this, depth+1, next_shortjump, next_medjump, next_longjump);
        edge.num_nodes = nn;
        edge.total_distance = td;
        num_nodes += nn;
        total_distance += td;
      } else {
        up_edge_ix = i;
      }
    }
    if (up_edge_ix != size_t(-1)) {
      up_edge_ = edges_[up_edge_ix];
      edges_[up_edge_ix] = edges_.back();
      edges_.pop_back();
    }

    total_distance_below_ = total_distance;

    num_nodes += 1;
    total_distance += num_nodes;
    return std::make_pair(num_nodes, total_distance);
  }

  void put_edge_info(int total_nodes, int num_nodes_above, i64 total_distance_above) {
    const i64 total_distance = total_distance_below_ + total_distance_above;

    for (auto& edge : edges_) {
      int nodes = total_nodes - edge.num_nodes;
      i64 distance = total_distance - edge.total_distance + nodes;
      edge.target->put_edge_info(total_nodes, nodes, distance);
    }

    up_edge_.num_nodes = num_nodes_above;
    up_edge_.total_distance = total_distance_above;
  }

#ifdef PRINT
  void show() const {
    std::cout << "\n[Node " << label_ << "]\n";
    std::cout << "Depth: " << depth_;
    if (up_edge_.target) {
      std::cout << " via " << up_edge_.target->label_;
      std::cout << "\nlong jump: " << longjump_->label_
        << " med jump: " << medjump_->label_
        << " short jump: " << shortjump_->label_ << '\n';
      std::cout << "Num nodes up: " << up_edge_.num_nodes << "\n";
      std::cout << "Total distance up: " << up_edge_.total_distance;
    }
    std::cout << '\n';
    std::cout << "Total distance below: " << total_distance_below_ << '\n';
    for (const auto& edge : edges_) {
      std::cout << "- Edge to " << edge.target->label_ << ":\n";
      std::cout << "| Num nodes: " << edge.num_nodes << "\n";
      std::cout << "| Total distance: " << edge.total_distance << "\n";
    }
  }
#endif
};

struct Nodes {
  Nodes(size_t N, size_t root) : nodes(N), root(root) {
#ifdef PRINT
    for (size_t i = 0; i < nodes.size(); ++i) nodes[i].label_ = i+1;
#endif
  }

  void add(size_t u, size_t v) {
    nodes[u].add_edge(&nodes[v]);
    nodes[v].add_edge(&nodes[u]);
  }

  void build_info() {
    nodes[root].mk_edge_info(nullptr, 0, nullptr, nullptr, nullptr);
    nodes[root].put_edge_info(nodes.size(), 0, 0);
  }

  i64 query(size_t a_ix, size_t b_ix) const {
    Node const* a = &nodes[a_ix];
    Node const* b = &nodes[b_ix];
    if (a->depth_ > b->depth_) std::swap(a, b);
    const size_t a_dp = a->depth_;
    const size_t b_dp = b->depth_;

    Node const* const join = [&]() {
      Node const* bp = b;
      while (((bp->depth_ - 1) & ~(LONG_JUMP - 1)) > a_dp) {
        bp = bp->longjump_;
      }
      while (((bp->depth_ - 1) & ~(MED_JUMP - 1)) > a_dp) {
        bp = bp->medjump_;
      }
      while (((bp->depth_ - 1) & ~(SHORT_JUMP - 1)) > a_dp) {
        bp = bp->shortjump_;
      }
      while (bp->depth_ > a_dp) {
        bp = bp->up_edge_.target;
      }
      Node const* ap = a;
      while (ap->longjump_ != bp->longjump_) {
        ap = ap->longjump_;
        bp = bp->longjump_;
      }
      while (ap->medjump_ != bp->medjump_) {
        ap = ap->medjump_;
        bp = bp->medjump_;
      }
      while (ap->shortjump_ != bp->shortjump_) {
        ap = ap->shortjump_;
        bp = bp->shortjump_;
      }
      while (ap != bp){
        ap = ap->up_edge_.target;
        bp = bp->up_edge_.target;
      }
      return ap;
    }();
    const size_t j_dp = join->depth_;
    const size_t dist = a_dp + b_dp - 2*j_dp;
    const size_t ad = dist / 2;
    const size_t bd = (dist - 1) / 2;

    Node const* const b_cut = [&]() {
      Node const* bp = b;
      size_t steps_left = bd;
      while (steps_left >= LONG_JUMP) {
        auto next_bp = bp->longjump_;
        steps_left -= bp->depth_ - next_bp->depth_;
        bp = next_bp;
      }
      while (steps_left >= MED_JUMP) {
        auto next_bp = bp->medjump_;
        steps_left -= bp->depth_ - next_bp->depth_;
        bp = next_bp;
      }
      while (steps_left >= SHORT_JUMP) {
        auto next_bp = bp->shortjump_;
        steps_left -= bp->depth_ - next_bp->depth_;
        bp = next_bp;
      }
      while (steps_left > 0) {
        bp = bp->up_edge_.target;
        --steps_left;
      }
      return bp;
    }();

    Node const* const a_cut = b_cut->up_edge_.target;
    const auto& be = b_cut->up_edge_;
    const auto& ae = [&]() -> EdgeInfo const& {
      return *std::lower_bound(a_cut->edges_.begin(), a_cut->edges_.end(), b_cut,
          [](const EdgeInfo& e, Node const* t) { return e.target < t; });
    }();

#ifdef PRINT
    std::cout << "A cut: " << a_cut->label_ << '\n';
    std::cout << "B cut: " << b_cut->label_ << '\n';
#endif

    const i64 a_contrib = a->up_edge_.total_distance + a->total_distance_below_;
    const i64 b_contrib = b->up_edge_.total_distance + b->total_distance_below_;

#ifdef PRINT
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

#ifdef PRINT
  void show_all() const {
    for (const auto& node : nodes) {
      node.show();
    }
    std::cout << "\nroot: " << nodes[root].label_ << "\n\n";
  }
#endif
};

void go() {
  READ(int, N);
  READ(int, Q);

#ifdef CUSTOM_ROOT
  READ(size_t, root);
#else
  const size_t root = 128'189 % N;
#endif

  Nodes nodes(N, root);

  for (int i = 0; i < N-1; ++i) {
#ifdef LINEAR
    nodes.add(i, i+1);
#else
    READ(int, u);
    READ(int, v);
    nodes.add(u-1, v-1);
#endif
  }

  nodes.build_info();
#ifdef PRINT
  nodes.show_all();
  std::cout.flush();
#endif

  for (int i = 0; i < Q; ++i) {
    READ(int, a);
    READ(int, b);
    std::cout << nodes.query(a-1, b-1) << '\n';
#ifdef PRINT
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
