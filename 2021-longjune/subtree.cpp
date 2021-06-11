#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using i64 = long long int;

template<typename T>
T read() { T t; std::cin >> t; return t; }
#define READ(T, x) T const x = read<T>()
#define MIN(x, y) do { x = std::min(x, y); } while(0)
#define MAX(x, y) do { x = std::max(x, y); } while(0)

struct Node {
  std::vector<Node*> children;
  int longest_path;
  bool used = false;

  void traverse(const Node* from);
  Node* flip_longest(Node* from);
  void walk_path();

  int top_child_path() const {
    return children.empty() ? 0 : children.front()->longest_path;
  }

#ifdef PRINT
  size_t label;

  void show(int depth) const {
    std::string prefix(depth, '|');
    std::cout << prefix << " Node " << label << (used ? " [used]" : "")
      << " longest path: " << longest_path << '\n';
    for (Node* child : children) {
      child->show(depth+1);
    }
    std::cout << prefix << "---\n";
  }
#endif
};

bool cmp(const Node* lhs, const Node* rhs) { return lhs->longest_path < rhs->longest_path; }

void Node::traverse(const Node* from) {
  size_t parent_edge_ix = -1;
  for (size_t i = 0; i < children.size(); ++i) {
    auto const child = children[i];
    if (child == from) {
      parent_edge_ix = i;
    } else {
      child->traverse(this);
    }
  }
  if (parent_edge_ix < children.size()) {
    std::swap(children[parent_edge_ix], children.back());
    children.pop_back();
  }

  std::make_heap(children.begin(), children.end(), cmp);

  longest_path = top_child_path() + 1;
}

Node* Node::flip_longest(Node* from) {
  if (children.empty()) {
    if (from) {
      children.push_back(from);
      longest_path = from->longest_path + 1;
    }
    return this;
  } else {
    std::pop_heap(children.begin(), children.end(), cmp);
    if (from) {
      std::swap(from, children.back());
      std::push_heap(children.begin(), children.end(), cmp);
    } else {
      from = children.back();
      children.pop_back();
    }
    longest_path = top_child_path() + 1;
    return from->flip_longest(this);
  }
}

// returns new longest path from this node
void Node::walk_path() {
  if (!children.empty()) {
    std::pop_heap(children.begin(), children.end(), cmp);
    children.back()->walk_path();
    if (children.back()->longest_path == 0) {
      children.pop_back();
    } else {
      std::push_heap(children.begin(), children.end(), cmp);
    }

    longest_path = top_child_path();
  }
  used = true;
}

struct Nodes {
  Nodes(size_t N) : nodes(N) {
#ifdef PRINT
    for (size_t i = 0; i < nodes.size(); ++i) { nodes[i].label = i+1; }
#endif
  }

  void add(size_t u, size_t v) {
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].children.push_back(&nodes[u]);
  }

  int solve(size_t start, int K) {
    if (K == 1) return 1;

    nodes[start].traverse(nullptr);

#ifdef PRINT
    std::cout << "Before flipping.\n";
    nodes[start].show(1);
    std::cout.flush();
#endif
    Node* root = nodes[start].flip_longest(nullptr);
#ifdef PRINT
    std::cout << "After flipping.\n";
    root->show(1);
    std::cout.flush();

    std::cout << "Marking root node as used.\n";
#endif

    root->used = true;
    root->longest_path -= 1;
    --K;
    int iterations = 1;
    while (K > 0) {
      int step = root->longest_path;
      K -= step;
      root->walk_path();
      while (root->children.size() == 1 && root->used) root = root->children[0];
#ifdef PRINT
      std::cout << "Removed " << step << " nodes.  New tree:\n";
      root->show(1);
#endif
      ++iterations;
    }
    return iterations;
  }

  std::vector<Node> nodes;
};

void go() {
  READ(int, N);
  READ(int, K);

  Nodes nodes(N);

  for (int i = 0; i < N-1; ++i) {
    READ(int, u);
    READ(int, v);
    nodes.add(u-1, v-1);
  }

  std::cout << nodes.solve(0, K) << '\n';
#ifdef PRINT
  std::cout.flush();
#endif
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
