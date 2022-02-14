#include <bspline.h>

BsplineCurve::BsplineCurve(std::vector<vec3> control_points, std::vector<float> knots, int degree)
{
    if (knots.size() != control_points.size() + degree + 1)
    {
        std::cout << "knots size and control points size do not match" << std::endl;
    }
    control_points_ = control_points;
    knots_ = knots;
    degree_ = degree;
}

BsplineCurve::BsplineCurve(int controlpoint_size, int degree)
{
    control_points_.resize(controlpoint_size);
    degree_ = degree;
    knots_.resize(controlpoint_size + degree + 1);
}

BsplineSurface::BsplineSurface(BsplineSurface& another)
{
    this->control_points_m_ = another.control_points_m_;
    this->control_points_n_ = another.control_points_n_;
    this->div_num = another.div_num;
    this->knots_m_ = another.knots_m_;
    this->knots_n_ = another.knots_n_;
    this->m_degree_ = another.m_degree_;
    this->n_degree_ = another.n_degree_;
}

void
BsplineCurve::Setcontrolpoint(int i, vec3 point)
{
    control_points_[i] = point;
}


Vertex
BsplineCurve::evaluate(std::vector<vec3> control_points, std::vector<float> knots, int degree, float t)
{
    Vertex final_point;
    int k = Findk(knots, t);

    std::vector<vec3> related_points;
    for (int i = 0; i < degree + 1; i++)
    {
        related_points.push_back(control_points[i+k-degree]);
    }
    for (int i = 1; i < degree + 1; i++)
    {
        for (int j = degree; j > i - 1; j--)
        {
            float a = (t - knots[k+j-degree])/(knots[k+j-(i-1)] - knots[k+j-degree]);
            related_points[j] = (1-a) * related_points[j-1] + a * related_points[j];
        }
        if (i == degree - 1)
        {
            final_point.tangent = glm::normalize(related_points[degree] - related_points[degree-1]);
        }
    }
    final_point.position = related_points[degree];
    return final_point;
}


Vertex
BsplineCurve::evaluate(float t)
{
    return evaluate(control_points_, knots_, degree_, t);
}

int
BsplineCurve::Findk(std::vector<float> knots, float t)
{
    for (int i = 0; i < knots.size() - 1; i++)
    {
        if (t == 1.0f)
        {
            if (t == knots[i+1]) 
                return i;
        }
        if (knots[i] <= t && t < knots[i+1])
            return i;
    }
    return -1;//wrong!
}


///BSplineSurface
BsplineSurface::BsplineSurface(int m, int n, int m_degree, int n_degree) : div_num(DIV_NUM)
{
    m_degree_ = m_degree;
    n_degree_ = n_degree;
    control_points_m_.resize(m);
    for (int i = 0; i < control_points_m_.size(); i++)
    {
        control_points_m_[i].resize(n);
    }
    knots_m_.resize(m + m_degree + 1);

    
    control_points_n_.resize(n);
    for (int i = 0; i < control_points_n_.size(); i++)
    {
        control_points_n_[i].resize(m);
    }
    knots_n_.resize(n + n_degree + 1);
}


void
BsplineSurface::Setcontrolpoint(int i, int j, vec3 point)
{
    control_points_m_[i][j] = point;
    control_points_n_[j][i] = point;
}

void
BsplineSurface::SetKnotM(int i, float u)
{
    knots_m_[i] = u;
}

void
BsplineSurface::SetKnotN(int i, float u)
{
    knots_n_[i] = u;
}


Vertex
BsplineSurface::evaluateM(float u, float v)
{
    BsplineCurve final_curve(control_points_m_.size(), m_degree_);
    final_curve.knots_ = knots_m_;
    for (int i = 0; i < control_points_m_.size(); i++)
    {
        BsplineCurve tempcurve(control_points_m_[i], knots_n_, n_degree_);
        final_curve.Setcontrolpoint(i,tempcurve.evaluate(v).position);
    }
    return final_curve.evaluate(u);
}

Vertex
BsplineSurface::evaluateN(float u, float v)
{
    BsplineCurve final_curve(control_points_n_.size(), n_degree_);
    final_curve.knots_ = knots_n_;
    for (int i = 0; i < control_points_n_.size(); i++)
    {
        BsplineCurve tempcurve(control_points_n_[i], knots_m_, m_degree_);
        final_curve.Setcontrolpoint(i,tempcurve.evaluate(v).position);
    }
    return final_curve.evaluate(u);
}

Object
BsplineSurface::generateObject()
{
    Object new_object;
    for (int i = 0; i < div_num + 1; i++)
    {
        float u = (float)i/(float)div_num;
        for (int j = 0; j < div_num + 1; j++)
        {
            float v = (float)j/(float)div_num;
            Vertex m_point = evaluateM(u,v);
            Vertex n_point = evaluateN(u,v);
            Vertex new_point;
            new_point.position = m_point.position;
            new_point.normal = glm::cross(m_point.tangent, n_point.tangent);
            new_object.vertices.push_back(new_point);
        }
    }

    ///face indices
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