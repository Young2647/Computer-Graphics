#ifndef _BSPLINE_H_
#define _BSPLINE_H_

#include "defines.h"
#include "object.h"
#include <vector>

#define DIV_NUM 50 

class BsplineCurve{
    public :
        std::vector<vec3> control_points_;
        std::vector<float> knots_;
        int degree_;
        
        BsplineCurve(std::vector<vec3> control_points, std::vector<float> knots, int degree);
        BsplineCurve(int controlpoint_size, int degree);
        void Setcontrolpoint(int i, vec3 point);
        Vertex evaluate(std::vector<vec3> control_points, std::vector<float> knots, int degree, float t);
        Vertex evaluate(float t);
        int Findk(std::vector<float>knots, float t);
};

class BsplineSurface{
    public :
        std::vector<std::vector<vec3>> control_points_m_;
        std::vector<std::vector<vec3>> control_points_n_;

        std::vector<float> knots_m_;
        std::vector<float> knots_n_;

        int m_degree_;
        int n_degree_;
        int div_num;
        BsplineSurface(int m, int n, int m_degree, int n_degree);    
        BsplineSurface(BsplineSurface& another);
        void Setcontrolpoint(int i, int j, vec3 point);
        void SetKnotM(int i, float u);
        void SetKnotN(int i, float u);
        Vertex evaluateM(float u, float v);
        Vertex evaluateN(float u, float v);
        Object generateObject();
};

#endif
