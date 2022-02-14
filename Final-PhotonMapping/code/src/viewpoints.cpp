#include <viewpoints.h>
#include <ray.h>
#include <geometry.h>

ViewPoint::ViewPoint(vec3 pos, vec3 N, vec3 color, double stgh, int x, int y)
 : C(pos), N(N), color(color), strength(stgh), x(x), y(y) {}


KdPointTreeNode::KdPointTreeNode(std::vector<std::shared_ptr<ViewPoint>> viewpoints, AABB box, int depth)
{
  this->box = box;
  if (viewpoints.size() < 8 || depth > 40) // 0 5 10 20 20 20
  {
    this->viewpoints = viewpoints;
    //std::cout << viewpoints.size() << ' ';
    leftChild = nullptr;
    rightChild = nullptr;
    return;
  }

  AABB leftSpace;
  AABB rightSpace;

  std::vector<float> centers;
  int split_axis = depth % 3;
  float split_value;

  for (int i = 0; i < viewpoints.size(); i++) {
    auto t = viewpoints[i];
    centers.push_back(t->C[split_axis]);
  }
  sort(centers.begin(), centers.end());
  split_value = centers[centers.size() / 2];

  //split_value = (box.ub[split_axis] + box.lb[split_axis]) / 2;

  leftSpace.lb = box.lb;
  leftSpace.ub = box.ub;
  leftSpace.ub[split_axis] = split_value;

  rightSpace.lb = box.lb;
  rightSpace.ub = box.ub;
  rightSpace.lb[split_axis] = split_value;

  // TODO: put the corresponding overlapping viewpoints
  std::vector<std::shared_ptr<ViewPoint>> leftViewpoints;
  std::vector<std::shared_ptr<ViewPoint>> rightViewpoints;

  for (auto& temp : viewpoints) {
    if (temp->C[split_axis] <= split_value) {
      leftViewpoints.push_back(temp);
    }
    if (temp->C[split_axis] > split_value) {
      rightViewpoints.push_back(temp);
    }
  }
  // recursively build left and right
  leftChild = new KdPointTreeNode(leftViewpoints, leftSpace, depth + 1);
  rightChild = new KdPointTreeNode(rightViewpoints, rightSpace, depth + 1);
}

void KdPointTreeNode::search(std::vector<std::shared_ptr<ViewPoint>>& result, const vec3& pos, double r)
{
  AABB tmp_box(pos - vec3(r, r, r), pos + vec3(r, r, r));
  if (!box.isOverlap(tmp_box)) return;
  if (isLeaf())
  {
    for (auto& p : viewpoints)
    {
      if ((p->C - pos).dot(p->C - pos) < r * r)
      {
        result.push_back(p);
      }
    }
    return;
  }
  leftChild->search(result, pos, r);
  rightChild->search(result, pos, r);
}

KdPointTreeNode::~KdPointTreeNode() {
  if (leftChild) {
    delete leftChild;
    leftChild = nullptr;
  }
  if (rightChild) {
    delete rightChild;
    rightChild = nullptr;
  }
}

KdPointTree::KdPointTree(const std::vector<std::shared_ptr<ViewPoint>>& viewpoints)
{
  AABB box;
  for (auto& p : viewpoints)
    box = AABB(box, p->C);
  root = std::move(std::make_unique<KdPointTreeNode>(viewpoints, box, 0));
}

void KdPointTree::search(std::vector<std::shared_ptr<ViewPoint>>& result, const vec3& pos, double r)
{
  if (root) return root->search(result, pos, r);
}