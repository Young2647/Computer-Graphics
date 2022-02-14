#ifndef VIEWPOINTS_H_
#define VIEWPOINTS_H_
#include <geometry.h>
#include <vector>
#include <memory>
#include <accel.h>


class ViewPoint
{
public:
  vec3 C, N;
  vec3 color;
  double strength;
  int x, y;

  ViewPoint() {}
  ViewPoint(vec3 pos, vec3 N, vec3 color, double stgh, int x, int y);
  friend class KdPointTreeNode;
  friend class KdPointTree;
};

class KdPointTreeNode
{
public:
  explicit KdPointTreeNode(std::vector<std::shared_ptr<ViewPoint>> viewpoints, AABB box, int depth);
  ~KdPointTreeNode();
  void search(std::vector<std::shared_ptr<ViewPoint>>& result, const vec3& pos, double r);
  [[nodiscard]] bool isLeaf() const { return !leftChild && !rightChild; }

private:
  KdPointTreeNode* leftChild, * rightChild;
  AABB box;
  std::vector<std::shared_ptr<ViewPoint>> viewpoints;
};




class KdPointTree
{
public:
  /* Creat the tree using a series of viewpoints */
  explicit KdPointTree(const std::vector<std::shared_ptr<ViewPoint>>& viewpoints);
  /* Return the points at pos with radius r in result */
  void search(std::vector<std::shared_ptr<ViewPoint>>& result, const vec3& pos, double r);

private:
  std::unique_ptr<KdPointTreeNode> root{};
};



#endif
