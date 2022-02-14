#ifndef CS171_HW3_INCLUDE_GEOMETRY_H_
#define CS171_HW3_INCLUDE_GEOMETRY_H_
#include <interaction.h>
#include <brdf.h>
/**
 * Base class of geometries
 */
class Geometry {
 public:
  Geometry() = default;
  virtual ~Geometry() = default;
  /**
   * ray-geometry intersect
   * @param[out] interaction output intersect infos
   * @param[in] ray the given ray
   * @return whether ray hit the geometry
   */
  virtual bool intersect(Interaction &interaction, const Ray &ray) const = 0;
  /**
   * set the mateial of a geometry
   * @param[in] mat the material
   */
  void setMaterial(std::shared_ptr<BRDF> mat);
  virtual vec3 getNormal() const = 0;
  virtual vec3 getCenter() const = 0;
 protected:
  std::shared_ptr<BRDF> material;  // material
};

/**
 * a struct to store triangle mesh
 */
struct TriangleMesh {
  /**
   * construct a triangle mesh
   * @param[in] indices index array
   * @param[in] nVertices vertices count
   * @param[in] p positions
   * @param[in] n normals
   * @param[in] uv texcoords
   */
  explicit TriangleMesh(const std::vector<int> &indices, int nVertices,
                        const std::vector<vec3> &p, const std::vector<vec3> &n,
                        const std::vector<vec2> &uv);

  const int nTriangles, nVertices;
  std::vector<int> indices;
  std::vector<vec3> p;   // positions
  std::vector<vec3> n;   // normals
  std::vector<vec2> uv;  // texcoords
};

/**
 * triangle geometry class
 */
class Triangle : public Geometry {
 public:
  /**
   * construct a triangle with a triangle in the mesh
   * @param[in] mesh the given triangle mesh
   * @param[in] v v[0], v[1], v[2] are the index value of the tringle
   * @param[in] mat material
   */
  explicit Triangle(std::shared_ptr<TriangleMesh> mesh, const int *v,
                    std::shared_ptr<BRDF> mat);
  /**
   * ray-triangle intersect
   * @param[out] interaction output intersect infos
   * @param[in] ray the given ray
   * @return whether ray hit the triangle
   */
  bool intersect(Interaction &interaction, const Ray &ray) const override;
  vec3 getCenter() const override{ return center; }
  [[nodiscard]] const vec3 &getVertex(int i) const {
    assert(0 <= i && i <= 2);
    return mesh->p[v[i]];
  }

  vec3 getNormal() const override{
      return (mesh->n[v[0]] + mesh->n[v[1]] + mesh->n[v[2]])/3;
  }

 
private:
  std::shared_ptr<TriangleMesh> mesh;
  const int *v;  // the index of the triangle (v[0], v[1], v[2])
  vec3 center; // the centre point coordinate for the triangle
};

// make a triangle mesh
std::vector<std::shared_ptr<Geometry>> makeTriangleMesh(
    const std::vector<int> &indices, int nVertices, const std::vector<vec3> &p,
    const std::vector<vec3> &n, const std::vector<vec2> &uv,
    const std::shared_ptr<BRDF> &mat);

// make a parallelogram
std::vector<std::shared_ptr<Geometry>> makeParallelogram(
    const vec3 &p0, const vec3 &s0, const vec3 &s1, const vec3 &normal,
    std::shared_ptr<BRDF> mat);
#endif  // CS171_HW3_INCLUDE_GEOMETRY_H_
