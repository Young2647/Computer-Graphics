#ifndef CS171_HW4_INCLUDE_ACCEL_H_
#define CS171_HW4_INCLUDE_ACCEL_H_
#include <geometry.h>
#include <vector>
#include <memory>

/**
 * Axis-Aligned Bounding Box
 */

struct AABB {
  vec3 lb{vec3::Zero()};
  vec3 ub{vec3::Zero()};

  AABB() = default;
  /* Construct AABB by coordinates of lower bound and upper bound */
  explicit AABB(Float lbX, Float lbY, Float lbZ, Float ubX, Float ubY,
                Float ubZ);
  explicit AABB(const vec3 &lb, const vec3 &ub);
  /* Construct AABB for a triangle */
  explicit AABB(const vec3 &v1, const vec3 &v2, const vec3 &v3);
  /* Construct AABB by merging two AABBs */
  explicit AABB(const AABB &a, const AABB &b);
  /* Construct AABB by adding with a point */
  explicit AABB(const AABB& a, const vec3 point);

  /* Get the AABB center */
  [[nodiscard]] vec3 getCenter() const;
  /* Get the length of a specified side on the AABB */
  [[nodiscard]] Float getDist(int c) const;
  /* Get the volume of the AABB */
  [[nodiscard]] Float getVolume() const;
  /* Check whether the AABB is overlapping with another AABB */
  [[nodiscard]] bool isOverlap(const AABB &a) const;
  /* Get the diagonal length */
  [[nodiscard]] Float diagonalLength() const;
  /* Test intersection with a ray */
  bool rayIntersection(const Ray &ray, Float &tIn, Float &tOut) const;
  bool pointIntersection(const vec3 pos);
  Float computeCost(int triangle_num);
};

class KdTreeNode {
 public:
  explicit KdTreeNode(std::vector<std::shared_ptr<Triangle>> triangles, AABB box, int depth);
  ~KdTreeNode();
  bool intersect(Interaction &interaction, const Ray &ray) const;
  [[nodiscard]] bool isLeaf() const { return !leftChild && !rightChild; }

 private:
  KdTreeNode *leftChild, *rightChild;
  AABB box;
  std::vector<std::shared_ptr<Triangle>> triangles;
};

class KdTreeAccel : public Geometry {
 public:
  explicit KdTreeAccel(const std::vector<std::shared_ptr<Triangle>> &triangles);
  bool intersect(Interaction &interaction, const Ray &ray) const override;
  vec3 getNormal() const override { return vec3::Zero(); }
  vec3 getCenter() const override { return vec3::Zero(); }
 private:
  std::unique_ptr<KdTreeNode> root{};
};
#endif  // CS171_HW4_INCLUDE_ACCEL_H_
