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
	
    vec3 E1 = v1 - v0;
    vec3 E2 = v2 - v0;
    vec3 T = ray.origin - v0;

    vec3 P = ray.direction.cross(E2);
    vec3 Q = T.cross(E1);
	Float det = P.dot(E1);
	if (abs(det) < SHADOW_EPS) return false;
    vec3 tuv = 1/(P.dot(E1)) * vec3(Q.dot(E2), P.dot(T), Q.dot(ray.direction));
    Float t = tuv.x();
    Float u = tuv.y();
    Float v = tuv.z();
    if (t >= ray.tMin && t <= ray.tMax && u >= 0 && v >= 0 && 1 - u - v >= 0)
	{
      interaction.entryDist = t;
      vec3 n0 = mesh->n[this->v[0]];
      vec3 n1 = mesh->n[this->v[1]];
      vec3 n2 = mesh->n[this->v[2]];
      vec2 uv0 = mesh->uv[this->v[0]];
      vec2 uv1 = mesh->uv[this->v[1]];
      vec2 uv2 = mesh->uv[this->v[2]];
      interaction.normal = (1 - u - v) * n0 + u * n1 + v * n2;
      interaction.uv = (1 - u - v) * uv0 + u * uv1 + v * uv2;
      interaction.entryPoint = (1 - u - v) * v0 + u * v1 + v *v2;
      if (material != nullptr) {
      	interaction.lightingModel = material->evaluate(interaction);
        interaction.normal = material->evalTextnormal(interaction,tbn);
      }
      interaction.type = Interaction::Type::GEOMETRY;
      return true;
    }
    return false;
}

/**
 * Geometry class
 */
void Geometry::setMaterial(std::shared_ptr<Material> newMat) {
  material = newMat;
}

Triangle::Triangle(std::shared_ptr<TriangleMesh> mesh, const int *v,
                   std::shared_ptr<Material> mat)
    : mesh(mesh), v(v) {
    setMaterial(mat);

    //world position
    vec3 p0 = mesh->p[v[0]];
    vec3 p1 = mesh->p[v[1]];
    vec3 p2 = mesh->p[v[2]];
    //uv coordinate
    vec2 uv0 = mesh->uv[v[0]];
    vec2 uv1 = mesh->uv[v[1]];
    vec2 uv2 = mesh->uv[v[2]];
    //normal
    vec3 n0 = mesh->n[v[0]];
    vec3 n1 = mesh->n[v[1]];
    vec3 n2 = mesh->n[v[2]];

    vec3 e1 = p1 - p0;
    vec3 e2 = p2 - p0;

    Float x1 = uv1.x() - uv0.x();
    Float y1 = uv1.y() - uv0.y();
    Float x2 = uv2.x() - uv0.x();
    Float y2 = uv2.y() - uv0.y();

    float f = 1 / (x1 * y2 - x2 * y1);
    Eigen::MatrixXf tb(3, 2);
    Eigen::MatrixXf e12(3, 2);
    e12 << e1, e2;
    Eigen::MatrixXf xy(2, 2);
    xy << y2, -x2,
        -y1, x1;
    tb = e12 * xy;
    vec3 t = tb.col(0);
    vec3 b = tb.col(1);
    vec3 n = (n0 + n1 + n2)/3;
    t = (t - t.dot(n) * n).normalized();
    b = t.cross(n).normalized();
    n = n.normalized();
    tbn << t, b, n;
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
    const std::shared_ptr<Material> &mat) {
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
    std::shared_ptr<Material> mat) {
  std::vector<int> index{0, 1, 2, 0, 2, 3};
  std::vector<vec3> position{p0, p0 + s0, p0 + s0 + s1, p0 + s1};
  std::vector<vec3> normals{normal, normal, normal, normal};
  std::vector<vec2> uv{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  return makeTriangleMesh(index, 4, position, normals, uv, mat);
}