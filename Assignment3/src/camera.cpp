#include <camera.h>
#include <ray.h>

Camera::Camera(const vec3 &pos, Float vFov, const vec2i &resolution,
               Float focalLen)
    : film(resolution),
      verticalFov(vFov),
      position(pos),
      focalLength(focalLen),
      forward(0, 0, 1),
      right(1, 0, 0),
      up(0, 1, 0) {}


/**
   * sample in pixel
   * @param[in] dx x in the film
   * @param[in] dy y in the film
   * @param[in] sample_num the num of samples per pixel
   */     
std::vector<vec2> 
Camera::samplePixel(int dx, int dy, int sample_num){
	std::vector<vec2> sample_points;
	Float rotation = radians(26.6f);
	Eigen::Matrix2f rot_mat = Eigen::Matrix2f::Zero();
	rot_mat(0,0) = sinf(rotation);
	rot_mat(0,1) = -cosf(rotation);
	rot_mat(1,0) = cosf(rotation);
	rot_mat(1,1) = sinf(rotation);
	vec2 y_dir = vec2(tan(rotation), 1).normalized();
	vec2 x_dir = vec2(1, -tan(rotation)).normalized();
	for (int i = 0; i < sample_num; i++)
	{
		for (int j = 0; j < sample_num; j++)
		{
			Float sample_x = ((Float)(i + 0.5)/(Float)sample_num - 0.5);
			Float sample_y = ((Float)(j + 0.5)/(Float)sample_num - 0.5);
			vec2 rotated_sample = rot_mat * vec2(sample_x, sample_y);
			if (rotated_sample.x() >= -0.5f && rotated_sample.x() <= 0.5f && rotated_sample.y() >= -0.5f && rotated_sample.y() <= 0.5f)
			{
				Float point_x = (Float)dx + 0.5f + rotated_sample.x();
				Float point_y = (Float)dy + 0.5f + rotated_sample.y();
				sample_points.push_back(vec2(point_x,point_y));
			}
		}
	}
	return sample_points;
}


/**
 * do camera look at
 * @param[in] lookAt look-at point
 * @param[in] refUp reference up vector
 */
void Camera::lookAt(const vec3 &lookAt, const vec3 &refUp) {
	this->forward = (position - lookAt).normalized();
	this->right = refUp.cross(forward).normalized();
	this->up = forward.cross(right).normalized();
	this->halfFovScale = tan(radians(verticalFov * 0.5f));
}

/**
 * generate a ray in the world space with given film sample (dx, dy)
 * @param[in] dx x in the film
 * @param[in] dy y in the film
 */
Ray Camera::generateRay(Float dx, Float dy) const {
  	vec3 position;
  	vec3 dir;

	Float width = film.resolution.x();
	Float height = film.resolution.y();
	Float screen_x, screen_y;
	screen_x = (2 * (dx)/width - 1) * film.getAspectRatio() * halfFovScale * focalLength;
	screen_y = (2 * (dy)/height - 1) * halfFovScale * focalLength;
	vec3 screenCenter = this->position - focalLength * forward;
	position = screenCenter + screen_x * right + screen_y * up;
	dir = (position - this->position).normalized();

  return Ray{position, dir};
}

void Camera::setPixel(int dx, int dy, const vec3 &value) {
  film.pixels[dy * film.resolution.x() + dx] = value;
}

const Film &Camera::getFilm() const { return film; }

std::shared_ptr<Camera> makeCamera(const vec3 &pos, Float vFov,
                                   const vec2i &resolution, Float focalLen) {
  return std::make_shared<Camera>(pos, vFov, resolution, focalLen);
}