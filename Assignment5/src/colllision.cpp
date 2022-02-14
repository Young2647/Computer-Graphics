#include <collision.h>
#include <utils.h>

/**
 * @brief This function checks whether the sphere collides the wall or not, and stores the collision information if necessary.
 * 
 * @param[in] wall 
 * @param[in] sphere 
 * @param[out] collision_info 
 * @return true 
 * @return false 
 */
bool CollisionHandler::Check(Wall &wall, Sphere &sphere, CollisionInfo &collision_info)
{
    return CheckParlgrmAndSphere(
        wall.GetX(), wall.GetS1(), wall.GetS2(),
        sphere.GetX(), sphere.GetRadius(), collision_info);
}

bool CollisionHandler::Check(Sphere& sphere1, Sphere& sphere2, CollisionInfo& collision_info)
{
    return CheckSphereAndSphere(sphere1.GetX(), sphere2.GetX(), sphere1.GetRadius(), sphere2.GetRadius(),collision_info);
}
/**
 * @brief This function handles the collision between the sphere and the wall.
 * 
 * @param[in] wall
 * @param[in] sphere 
 */
void CollisionHandler::Handle(Wall &wall, Sphere &sphere)
{
    CollisionInfo temp_collision_info;
    if (Check(wall, sphere, temp_collision_info))
    {
        vec3 sphere_speed = sphere.GetV();
        //vec3 normal = glm::normalize(glm::cross(wall.GetS1(), wall.GetS2()));
        vec3 normal = glm::normalize(sphere.GetX() - temp_collision_info.collision_point);
        float vrel = glm::dot(sphere_speed, normal);// relative speed on normal direction
        vec3 v_normal = vrel * normal;
        vec3 v_tan = sphere_speed - vrel * normal;
        /*vec3 normal_tan = v_tan == vec3(0, 0, 0) ? vec3(0,0,0) : glm::normalize(v_tan);
        float vrel_tan = glm::dot(v_tan, normal_tan);*/
        if (vrel > 0.1) return; //moving away
        if (vrel > -0.1)// resting contact
        {
            Force sphere_press_force;
            for (Force force : sphere.constant_forces) sphere_press_force.F += force.F;
            for (Force force : sphere.transient_forces) sphere_press_force.F += force.F;
            sphere_press_force.F = glm::dot(sphere_press_force.F, normal) * normal;
            Force N;
            N.F = -sphere_press_force.F; // support force
            sphere.transient_forces.push_back(N);
            vec3 numerator = -v_normal;
            float term1 = 1 / sphere.GetM();
            float term2 = 1 / wall.GetM();
            vec3 j = numerator / (term1 + term2);
            Force collision_force;
            collision_force.F = j / dt;
            collision_force.r = vec3(0, 0, 0);
            sphere.transient_forces.push_back(collision_force);

            if (glm::dot(N.F, normal) > 0.1)
            {
                vec3 tan_numerator = -(1 - tangent_epsilon) * v_tan;
                float term1 = 1 / sphere.GetM();
                float term2 = 1 / wall.GetM();
                vec3 j_tan = tan_numerator / (term1 + term2);
                Force tan_force;
                tan_force.F = j_tan / dt;
                //tan_force.F = vec3(0,0,0);
                sphere.transient_forces.push_back(tan_force);
            }
        }
        else
        {
            Force sphere_press_force;
            for (Force force : sphere.constant_forces) sphere_press_force.F += force.F;
            for (Force force : sphere.transient_forces) sphere_press_force.F += force.F;
            sphere_press_force.F = glm::dot(sphere_press_force.F, normal) * normal;
            Force N;
            N.F = -sphere_press_force.F; // support force

            vec3 numerator = -(1 + normal_epsilon) * v_normal;
            float term1 = 1 / sphere.GetM();
            float term2 = 1 / wall.GetM();
            vec3 j = numerator / (term1 + term2);
            Force collision_force;
            collision_force.F = j / dt;
            collision_force.r = vec3(0, 0, 0);
            sphere.transient_forces.push_back(collision_force); // collision force


            if (glm::dot(N.F, normal) > 0.1)
            {
                vec3 tan_numerator = -(1 - tangent_epsilon) * v_tan;
                float term1 = 1 / sphere.GetM();
                float term2 = 1 / wall.GetM();
                vec3 j_tan = tan_numerator / (term1 + term2);
                Force tan_force;
                tan_force.F = j_tan / dt;
                //tan_force.F = vec3(0,0,0);
                sphere.transient_forces.push_back(tan_force);
            }
        }
    }

    /// TODO: Handle the colliding contact between the sphere and the wall.
}

void CollisionHandler::Handle(Sphere & sphere1, Sphere & sphere2)
{
    CollisionInfo temp_collision_info;
    if (Check(sphere1, sphere2, temp_collision_info))
    {
        vec3 sphere1_speed = sphere1.GetV();
        vec3 sphere2_speed = sphere2.GetV();
        vec3 collision_normal = glm::normalize(sphere1.GetX() - sphere2.GetX());
        float vrel = glm::dot(sphere1_speed - sphere2_speed, collision_normal);
        vec3 v_normal = vrel * collision_normal;
        vec3 v_sphere1_normal = glm::dot(sphere1_speed, collision_normal) * collision_normal;
        vec3 v_sphere2_normal = glm::dot(sphere2_speed, collision_normal) * collision_normal;
        vec3 v_sphere1_tan = sphere1_speed -  v_sphere1_normal;
        vec3 v_sphere2_tan = sphere2_speed - v_sphere2_normal;
        if (vrel > 0.1) return; // moving away
        if (vrel > -0.1) //resting contact
        {
            Force sphere1_press_force;
            for (Force force : sphere1.constant_forces) sphere1_press_force.F += force.F;
            for (Force force : sphere1.transient_forces) sphere1_press_force.F += force.F;
            sphere1_press_force.F = glm::dot(sphere1_press_force.F, collision_normal) * collision_normal;
            Force N2to1;
            N2to1.F = -sphere1_press_force.F; // support force

            Force sphere2_press_force;
            for (Force force : sphere2.constant_forces) sphere2_press_force.F += force.F;
            for (Force force : sphere2.transient_forces) sphere2_press_force.F += force.F;
            sphere2_press_force.F = glm::dot(sphere2_press_force.F, collision_normal) * collision_normal;
            Force N1to2;
            N1to2.F = -sphere2_press_force.F; // support force

            sphere1.transient_forces.push_back(N2to1);
            sphere2.transient_forces.push_back(N1to2);

            //vec3 numerator = -v_normal;
            //float term1 = 1 / sphere1.GetM();
            //float term2 = 1 / sphere2.GetM();
            //vec3 j = numerator / (term1 + term2);
            //Force collision_force;
            //collision_force.F = j / dt;
            //collision_force.r = vec3(0, 0, 0);
            //sphere1.transient_forces.push_back(collision_force);
            //sphere2.transient_forces.push_back(collision_force.back_force());
        }
        else
        {
            vec3 numerator = -(1 + normal_epsilon) * v_normal;
            float term1 = 1 / sphere1.GetM();
            float term2 = 1 / sphere2.GetM();
            vec3 j = numerator / (term1 + term2)/2.0f;
            Force collision_force;
            collision_force.F = j / dt;
            collision_force.r = vec3(0, 0, 0);
            sphere1.transient_forces.push_back(collision_force); // collision force
            sphere2.transient_forces.push_back(collision_force.back_force());
            //vec3 numerator1 = -(1 + normal_epsilon) * v_sphere1_normal;
            //float term1 = 1 / sphere1.GetM();
            //vec3 j1 = numerator1 / term1;
            //Force collision_force1;
            //collision_force1.F = j1 / dt;
            //collision_force1.r = vec3(0, 0, 0);
            //sphere1.transient_forces.push_back(collision_force1); // collision force


            //vec3 numerator2 = -(1 + normal_epsilon) * v_sphere2_normal;
            //float term2 = 1 / sphere2.GetM();
            //vec3 j2 = numerator2 / term2;
            //Force collision_force2;
            //collision_force2.F = j2 / dt;
            //collision_force2.r = vec3(0, 0, 0);
            //sphere2.transient_forces.push_back(collision_force2); // collision force
        }
    }
}

/**
 * @brief This function checks whether the parallelogram collides with the sphere.
 *        The collision information will be store if there exists the collision.
 * @param[in] parlgrm_x 
 * @param[in] parlgrm_s1 
 * @param[in] parlgrm_s2 
 * @param[in] sphere_x 
 * @param[in] sphere_r 
 * @param[out] collision_info 
 * @return true 
 * @return false 
 */

bool CollisionHandler::Notinparallelogram(glm::vec3 parlgrm_x,
    glm::vec3 parlgrm_s1,
    glm::vec3 parlgrm_s2,
    glm::vec3 circle_x,
    float circle_r)
{
    vec3 topleft = parlgrm_x - parlgrm_s1 / 2.0f + parlgrm_s2 / 2.0f;
    vec3 topright = parlgrm_x + parlgrm_s1 / 2.0f + parlgrm_s2 / 2.0f;
    vec3 bottomleft = parlgrm_x - parlgrm_s1 / 2.0f - parlgrm_s2 / 2.0f;
    vec3 bottomright = parlgrm_x + parlgrm_s1 / 2.0f - parlgrm_s2 / 2.0f;
    float half_diag_dist = glm::distance(topright, parlgrm_x);
    
    vec3 upline = parlgrm_s1;
    vec3 proj_up = glm::dot(topright - circle_x, upline) * upline;
    vec3 orthorg_up = topright - circle_x - proj_up;
    if (glm::normalize(orthorg_up) == glm::normalize(parlgrm_s2)) return true; //circle is in
    else if (glm::distance(orthorg_up, vec3(0, 0, 0)) < circle_r) return true; //circle is in

    vec3 downline = parlgrm_s1;
    vec3 proj_down = glm::dot(bottomright - circle_x, downline) * downline;
    vec3 orthorg_down = bottomright - circle_x - proj_down;
    if (glm::normalize(orthorg_down) == glm::normalize(-parlgrm_s2)) return true; //circle is in
    else if (glm::distance(orthorg_down, vec3(0, 0, 0)) < circle_r) return true; //circle is in

    vec3 leftline = parlgrm_s2;
    vec3 proj_left = glm::dot(topleft - circle_x, leftline) * leftline;
    vec3 orthorg_left = topleft - circle_x - proj_left;
    if (glm::normalize(orthorg_left) == glm::normalize(-parlgrm_s1)) return true; //circle is in
    else if (glm::distance(orthorg_left, vec3(0, 0, 0)) < circle_r) return true; //circle is in

    vec3 rightline = parlgrm_s2;
    vec3 proj_right = glm::dot(topright - circle_x, rightline) * rightline;
    vec3 orthorg_right = topright - circle_x - proj_right;
    if (glm::normalize(orthorg_right) == glm::normalize(parlgrm_s1)) return true; //circle is in
    else if (glm::distance(orthorg_right, vec3(0, 0, 0)) < circle_r) return true; //circle is in


    return false; // all not satisfied, return false
}

bool CollisionHandler::CheckParlgrmAndSphere(
    glm::vec3 parlgrm_x,
    glm::vec3 parlgrm_s1,
    glm::vec3 parlgrm_s2,
    glm::vec3 sphere_x,
    float sphere_r,
    CollisionInfo &collision_info)
{
    /// TODO: Check the collision between the parallelogram and the sphere.
    vec3 normal = glm::normalize(glm::cross(parlgrm_s1, parlgrm_s2));
    vec3 temp_line = parlgrm_x - sphere_x;
    float dist = abs(glm::dot(normal, temp_line));

    if (dist - sphere_r > 0) // not intersect
    {
        collision_info.is_collided = false;
        return false;
    }
    else
    {
        float t = glm::dot(normal, temp_line);
        collision_info.collision_point = sphere_x + t * normal; // collision_point = o + td
        float circle_r = sqrt(sphere_r*sphere_r - t * t);
        if (Notinparallelogram(parlgrm_x, parlgrm_s1, parlgrm_s2, collision_info.collision_point, circle_r)) return false; // not in paralleogram
        collision_info.is_collided = true;
        //collision_info.collision_degree = acos(abs(t/sphere_r)); //cos_theta = distance / sphere_r
        collision_info.collision_degree = 1 - abs(t / sphere_r);
        return true;
    }
}

bool CollisionHandler::CheckSphereAndSphere(glm::vec3 sphere1_x,
    glm::vec3 sphere2_x,
    float sphere1_r,
    float sphere2_r,
    CollisionInfo& collision_info)
{
    float distance = abs(glm::distance(sphere1_x, sphere2_x));
    if (distance == 0) return false; // the same sphere
    if (distance - sphere1_r - sphere2_r > 0) // not intersect
    {
        collision_info.is_collided = false;
        return false;
    }
    else
    {
        collision_info.collision_point = 0.5f * (sphere1_x + sphere2_x);
        collision_info.is_collided = true;
        collision_info.collision_degree = 1 - (distance / (sphere1_r + sphere2_r));
        return true;
    }
}