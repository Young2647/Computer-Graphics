#include <geometry.h>
#include <ray.h>
#include <interaction.h>

/**
 * ray-triangle intersect
 * @param[out] interaction output intersect infos
 * @param[in] ray the given ray
 * @return whether ray hit the triangle
 */
bool Triangle::intersect(Interaction &interaction, const Ray &ray) const {
  const vec3 &v0 = mesh->p[v[0]];
  const vec3 &v1 = mesh->p[v[1]];
  const vec3 &v2 = mesh->p[v[2]];
  vec3 v0v1 = v1 - v0;
  vec3 v0v2 = v2 - v0;
  vec3 pvec = ray.direction.cross(v0v2);
  Float det = v0v1.dot(pvec);

  if (det == 0) return false;

  Float invDet = 1.0 / det;

  vec3 tvec = ray.origin - v0;
  Float u = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1) return false;
  vec3 qvec = tvec.cross(v0v1);
  Float v = ray.direction.dot(qvec) * invDet;
  if (v < 0 || u + v > 1) return false;
  Float t = v0v2.dot(qvec) * invDet;
  if (t < ray.tMin || t > ray.tMax) return false;

  interaction.entryDist = t;
  interaction.entryPoint = ray.getPoint(t);
  interaction.normal = (u * mesh->n[this->v[1]] + v * mesh->n[this->v[2]] +
                        (1 - u - v) * mesh->n[this->v[0]])
                           .normalized();

  interaction.brdf = material;
  interaction.type = Interaction::Type::GEOMETRY;

  return true;
}

/**
 * Geometry class
 */
void Geometry::setMaterial(std::shared_ptr<BRDF> newMat) { material = newMat; }

Triangle::Triangle(std::shared_ptr<TriangleMesh> mesh, const int *v,
                   std::shared_ptr<BRDF> mat)
    : mesh(mesh), v(v) {
  setMaterial(mat);
  this->center = (mesh->p[v[0]] + mesh->p[v[1]] + mesh->p[v[2]])/3;
}

TriangleMesh::TriangleMesh(const std::vector<int> &indices, int nVertices,
                           const std::vector<vec3> &p,
                           const std::vector<vec3> &n,
                           const std::vector<vec2> &uv)
    : nTriangles(static_cast<int>(indices.size() / 3)), nVertices(nVertices) {
  this->indices = indices;
  this->p.resize(nVertices);
  for (int i = 0; i < nVertices; ++i)
    this->p[i] = p[i];  // objectToWorld(pos[i]);
  if (!n.empty()) {
    this->n.resize(nVertices);
    for (int i = 0; i < nVertices; ++i)
      this->n[i] = n[i];  // objectToWorld(normal[i]);
  }
  if (!uv.empty()) {
    this->uv.assign(uv.begin(), uv.begin() + nVertices);
  }
}

std::vector<std::shared_ptr<Geometry>> makeTriangleMesh(
    const std::vector<int> &indices, int nVertices, const std::vector<vec3> &p,
    const std::vector<vec3> &n, const std::vector<vec2> &uv,
    const std::shared_ptr<BRDF> &mat) {
  std::vector<std::shared_ptr<Geometry>> ret;
  auto mesh = std::make_shared<TriangleMesh>(indices, nVertices, p, n, uv);
  ret.reserve(mesh->nTriangles);
  for (int i = 0; i < mesh->nTriangles; ++i) {
    ret.push_back(
        std::make_shared<Triangle>(mesh, mesh->indices.data() + 3 * i, mat));
  }
  return ret;
}

std::vector<std::shared_ptr<Geometry>> makeParallelogram(
    const vec3 &p0, const vec3 &s0, const vec3 &s1, const vec3 &normal,
    std::shared_ptr<BRDF> mat) {
  std::vector<int> index{0, 1, 2, 0, 2, 3};
  std::vector<vec3> position{p0, p0 + s0, p0 + s0 + s1, p0 + s1};
  std::vector<vec3> normals{normal, normal, normal, normal};
  std::vector<vec2> uv{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  return makeTriangleMesh(index, 4, position, normals, uv, mat);
}
