#include <accel.h>
#include <ray.h>
#include <geometry.h>

//functions to sort
bool xcmp(const std::shared_ptr<Triangle>& t1, const std::shared_ptr<Triangle>& t2) { return t1.get()->getCenter().x() < t2.get()->getCenter().x(); } // compare along x-axis
bool ycmp(const std::shared_ptr<Triangle>& t1, const std::shared_ptr<Triangle>& t2) { return t1.get()->getCenter().y() < t2.get()->getCenter().y(); } // compare along y-axis
bool zcmp(const std::shared_ptr<Triangle>& t1, const std::shared_ptr<Triangle>& t2) { return t1.get()->getCenter().z() < t2.get()->getCenter().z(); } // compare along z-axis


/**
 * construct kd tree with given triangles
 * @param triangles a array of triangles
 */
KdTreeNode::KdTreeNode(std::vector<std::shared_ptr<Triangle>> triangles,
    AABB box, int depth) {
    // the space of the current node
    this->box = box;
    // stop dividing when the number of triangles is small enough
    // or stop when depth is too large
    // 8 and 40 are just examples, it may not be good enough
    if (triangles.size() < 8 || depth > 40) {
        this->triangles = triangles;
        this->leftChild = NULL;
        this->rightChild = NULL;
        // TODO: store triangles into the leaf node
        return;
    }


    // TODO: divide the space into two parts according to one specific dimension
    AABB leftSpace;
    AABB rightSpace;


    // TODO: put the corresponding overlapping triangles
    std::vector<std::shared_ptr<Triangle>> leftTriangles;
    std::vector<std::shared_ptr<Triangle>> rightTriangles;

    Float overall_minCost = INF; // the minimun cost along all axis
    int overall_best_split; // the best split along all axis
    int best_axis; // the axis that gives the best division
    for (int axis = 0; axis < 3; axis++)
    {
        if (axis == 0) std::sort(triangles.begin(), triangles.end(), xcmp);
        if (axis == 1) std::sort(triangles.begin(), triangles.end(), ycmp);
        if (axis == 2) std::sort(triangles.begin(), triangles.end(), zcmp);

        //construct leftSpace and rightSpace
        std::vector<AABB> leftspaces;
        std::vector<AABB> rightspaces;
        leftspaces.resize(triangles.size());
        rightspaces.resize(triangles.size());
        for (int i = 0; i < triangles.size(); i++)
        {
            auto left_triangle = triangles[i];
            if (i == 0)
            {
                leftspaces[i] = AABB(left_triangle.get()->getVertex(0), left_triangle.get()->getVertex(1), left_triangle.get()->getVertex(2));
            }
            else
            {
                AABB tmp_left(left_triangle.get()->getVertex(0), left_triangle.get()->getVertex(1), left_triangle.get()->getVertex(2));
                leftspaces[i] = AABB(leftspaces[i - 1], tmp_left);
            }


            auto right_triangle = triangles[triangles.size() - 1 - i];
            if (i == 0)
            {
                rightspaces[triangles.size() - 1 - i] = AABB(right_triangle.get()->getVertex(0), right_triangle.get()->getVertex(1), right_triangle.get()->getVertex(2));
            }
            else
            {
                AABB tmp_right(right_triangle.get()->getVertex(0), right_triangle.get()->getVertex(1), right_triangle.get()->getVertex(2));
                rightspaces[triangles.size() - 1 - i] = AABB(rightspaces[triangles.size() - i], tmp_right);
            }
        }

        //compute the cost for each space division and choose the best division
        Float minCost = INF;
        int best_split = -1;
        for (int i = 0; i < triangles.size(); i++)
        {
            Float cost = leftspaces[i].computeCost(i + 1) + rightspaces[i].computeCost(triangles.size() - 1 - i);
            if (cost < minCost)
            {
                minCost = cost;
                best_split = i;
            }
        }
        if (minCost < overall_minCost)
        {
            overall_minCost = minCost;
            best_axis = axis;
            overall_best_split = best_split;
            leftSpace = leftspaces[overall_best_split]; // we get the final leftSpace now
            rightSpace = rightspaces[overall_best_split]; // we get the final rightSpace now
            leftTriangles.assign(triangles.begin(), triangles.begin() + overall_best_split + 1);
            rightTriangles.assign(triangles.begin() + overall_best_split + 1, triangles.end());
        }
    }


    // recursively build left and right
    leftChild = new KdTreeNode(leftTriangles, leftSpace, depth + 1);
    rightChild = new KdTreeNode(rightTriangles, rightSpace, depth + 1);
}

bool KdTreeNode::intersect(Interaction& interaction, const Ray& ray) const {
    // TODO: first check whether ray hit the bounding box
    Float tIn, tOut;
    if (!this->box.rayIntersection(ray, tIn, tOut)) return false; // not intersect with bounding box, return false directly.
    if (isLeaf()) {
        Interaction final_interaction;
        bool intersect = false;
        final_interaction.entryDist = INF;
        for (auto triangle : triangles)
        {
            if (triangle.get()->intersect(interaction, ray))
            {
                intersect = true;
                if (interaction.entryDist < final_interaction.entryDist) // it is the nearest triangle in the bounding box
                {
                    final_interaction = interaction;
                }
            }
        }
        interaction = final_interaction;
        return intersect;
        // all triangles are stored in leaf nodes
        // TODO: do intersect with triangles
    }
    // TODO: recursively test intersection with left and right
    bool left_result = leftChild->intersect(interaction, ray);
    Interaction tmp_left_interaction = interaction;
    bool right_result = rightChild->intersect(interaction, ray);
    Interaction tmp_right_interaction = interaction;
    if (left_result && right_result)
    {
        if (tmp_left_interaction.entryDist < tmp_right_interaction.entryDist && left_result)
            interaction = tmp_left_interaction;
        else
            interaction = tmp_right_interaction;
    }
    else if (left_result) interaction = tmp_left_interaction;
    else if (right_result) interaction = tmp_right_interaction;
    return left_result || right_result;
}

KdTreeNode::~KdTreeNode() {
    if (leftChild) {
        delete leftChild;
        leftChild = nullptr;
    }
    if (rightChild) {
        delete rightChild;
        rightChild = nullptr;
    }
}

KdTreeAccel::KdTreeAccel(
    const std::vector<std::shared_ptr<Triangle>>& triangles) {
    AABB box;
    for (auto& tri : triangles)
        box = AABB(box,
            AABB(tri->getVertex(0), tri->getVertex(1), tri->getVertex(2)));
    root = std::move(std::make_unique<KdTreeNode>(triangles, box, 0));
}

bool KdTreeAccel::intersect(Interaction& interaction, const Ray& ray) const {
    if (root) return root->intersect(interaction, ray);
    return false;
}

AABB::AABB(Float lbX, Float lbY, Float lbZ, Float ubX, Float ubY, Float ubZ) {
    lb = vec3(lbX, lbY, lbZ);
    ub = vec3(ubX, ubY, ubZ);
}

AABB::AABB(const vec3& lb, const vec3& ub) : lb(lb), ub(ub) {}

AABB::AABB(const vec3& v1, const vec3& v2, const vec3& v3) {
    lb = v1.cwiseMin(v2).cwiseMin(v3);
    ub = v1.cwiseMax(v2).cwiseMax(v3);
}

AABB::AABB(const AABB& a, const AABB& b) {
    lb = vec3(a.lb.cwiseMin(b.lb));
    ub = vec3(a.ub.cwiseMax(b.ub));
}

AABB::AABB(const AABB& a, const vec3 point)
{
    lb = vec3(a.lb.cwiseMin(point));
    ub = vec3(a.ub.cwiseMax(point));
}

vec3 AABB::getCenter() const { return (lb + ub) / 2; }

Float AABB::getDist(int c) const { return ub[c] - lb[c]; }

Float AABB::getVolume() const { return getDist(2) * getDist(1) * getDist(0); }

bool AABB::isOverlap(const AABB& a) const {
    return ((a.lb[0] >= this->lb[0] && a.lb[0] <= this->ub[0]) ||
        (this->lb[0] >= a.lb[0] && this->lb[0] <= a.ub[0])) &&
        ((a.lb[1] >= this->lb[1] && a.lb[1] <= this->ub[1]) ||
            (this->lb[1] >= a.lb[1] && this->lb[1] <= a.ub[1])) &&
        ((a.lb[2] >= this->lb[2] && a.lb[2] <= this->ub[2]) ||
            (this->lb[2] >= a.lb[2] && this->lb[2] <= a.ub[2]));
}

Float AABB::diagonalLength() const { return (ub - lb).norm(); }

bool AABB::rayIntersection(const Ray& ray, Float& tIn, Float& tOut) const {
    Float dirX = (ray.direction[0] == 0) ? INF : Float(1) / ray.direction[0];
    Float dirY = (ray.direction[1] == 0) ? INF : Float(1) / ray.direction[1];
    Float dirZ = (ray.direction[2] == 0) ? INF : Float(1) / ray.direction[2];

    Float tx1 = (lb[0] - ray.origin[0]) * dirX;
    Float tx2 = (ub[0] - ray.origin[0]) * dirX;
    Float ty1 = (lb[1] - ray.origin[1]) * dirY;
    Float ty2 = (ub[1] - ray.origin[1]) * dirY;
    Float tz1 = (lb[2] - ray.origin[2]) * dirZ;
    Float tz2 = (ub[2] - ray.origin[2]) * dirZ;

    tIn = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)),
        std::min(tz1, tz2));
    tOut = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)),
        std::max(tz1, tz2));

    /* When tOut < 0 and the ray is intersecting with AABB, the whole AABB is
     * behind us */
    if (tOut < 0) {
        return false;
    }

    return tOut >= tIn;
}

bool AABB::pointIntersection(const vec3 pos)
{
    return (pos[0] > lb[0] && pos[0] <= ub[0] && pos[1] > lb[1] && pos[1] <= ub[1] && pos[2] > lb[2] && pos[2] <= ub[2]);
}

Float AABB::computeCost(int triangle_num)
{
    Float lenx = getDist(0);
    Float leny = getDist(1);
    Float lenz = getDist(2);
    Float P = 2 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
    return P * (Float)triangle_num;
}