#ifndef _BEZIER_H_
#define _BEZIER_H_
#include "defines.h"
#include <object.h>

#include <vector>

#define DIV_NUM 50

class BezierCurve {
 public:
  std::vector<vec3> control_points_;
  int div_num;
  BezierCurve(int m);
  BezierCurve(std::vector<vec3>& control_points);

  void setControlPoint(int i, vec3 point);
  Vertex evaluate(std::vector<vec3>& control_points, float t);
  Vertex evaluate(float t);
  Object generateObject();
};

class BezierSurface {
 public:
  std::vector<std::vector<vec3>> control_points_m_;
  std::vector<std::vector<vec3>> control_points_n_;
  int div_num;

  BezierSurface(int m, int n);
  void setControlPoint(int i, int j, vec3 point);
  Vertex evaluate(std::vector<std::vector<vec3>>& control_points, float u, float v);
  Object generateObject();
};

#endif