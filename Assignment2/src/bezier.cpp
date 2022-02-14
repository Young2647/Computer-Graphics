#include <bezier.h>
#include <utils.h>
#include <vector>

BezierCurve::BezierCurve(int m):div_num(DIV_NUM) { control_points_.resize(m); }

BezierCurve::BezierCurve(std::vector<vec3>& control_points):div_num(DIV_NUM) {
  control_points_ = control_points;
}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t) {
  /*
  if (control_points.size() == 2)
  {
    Vertex final_point;
    final_point.position = (1-t) * control_points[0] + t * control_points[1];
    final_point.tangent = control_points[1] - control_points[0];
    return final_point;
  }
  else
  {
    std::vector<vec3> new_control_points;
    for (int i = 0; i < control_points.size() - 1; i++)
    {
      vec3 new_point;
      new_point = (1-t) * control_points[i] + t * control_points[i+1];
      new_control_points.push_back(new_point);
    }
    return evaluate(new_control_points,t);
  }
  */
 std::vector<vec3> new_control_points = control_points;
 Vertex final_point;
 for(int k = 1; k < control_points.size(); k++)
 {
   for (int i = 0; i < control_points.size()-k; i++)
   {
      new_control_points[i] = (1-t) * new_control_points[i] + t * new_control_points[i+1];
   }
   if (k == control_points.size()-2)
   {
     final_point.tangent = glm::normalize(new_control_points[1] - new_control_points[0]);
   }
 }
 final_point.position = new_control_points[0];
 return final_point;
}

Vertex BezierCurve::evaluate(float t) { return evaluate(control_points_, t); }

/**
 * TODO: generate an Object of the current Bezier curve
 */
Object BezierCurve::generateObject() {
    Object new_object;
    for (int i = 0; i < div_num; i++)
    {
        float t = (float)i / (float)div_num;
        new_object.vertices.push_back(evaluate(t));
    }
    return new_object;
}

BezierSurface::BezierSurface(int m, int n):div_num(DIV_NUM) {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) {
    sub_vec.resize(n);
  }
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) {
    sub_vec.resize(m);
  }
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

/**
 * TODO: evaluate the point at (u, v) with the given control points
 */
Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>& control_points,
                               float u, float v) {
  BezierCurve order1_curve(control_points.size());
  for (int i = 0; i < control_points.size(); i++)
  {
    BezierCurve tempcurve(control_points[i]);
    order1_curve.setControlPoint(i, tempcurve.evaluate(v).position);
  }
  return order1_curve.evaluate(u);                              
}



/**
 * TODO: generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() {
    Object new_object;
    for (int i = 0; i <= div_num; i++)
    {
        float u = (float)i / (float)div_num;
        for (int j = 0; j <= div_num; j++)
        {
            float v = (float)j / (float)div_num;
            Vertex m_point = evaluate(control_points_m_, u, v);
            Vertex n_point = evaluate(control_points_n_, v, u);
            Vertex new_point;
            new_point.position = m_point.position;
            new_point.normal = glm::cross(m_point.tangent, n_point.tangent);
            new_object.vertices.push_back(new_point);
        }
    }
    ///add face indices
    int point_num = div_num + 1;
    for (int i = 0;i < point_num - 1; i++)
    {
        for (int j = 0; j < point_num - 1; j++)
        {
        new_object.indices.push_back(i * point_num + j + 1);
        new_object.indices.push_back(i * point_num + j);
        new_object.indices.push_back((i+1) * point_num + j);///1st triangle

        new_object.indices.push_back(i * point_num + j + 1);
        new_object.indices.push_back((i+1) * point_num + j);
        new_object.indices.push_back((i+1) * point_num + j + 1);///2nd triangle
        }
    }
    return new_object;
}