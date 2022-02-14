#ifndef CS171_HW3_INCLUDE_CAMERA_H_
#define CS171_HW3_INCLUDE_CAMERA_H_
#include <core.h>
#include <film.h>

/**
 * Perspective camera class
 */
class Camera {
 public:
  /**
   * construct a camera
   * @param[in] pos camera's position
   * @param[in] vFov vertical fov
   * @param[in] resolution film resolution
   */
  Camera(const vec3 &pos, Float vFov, const vec2i &resolution,
         Float focalLen = 1);

  /**
  * sample in pixel
  * @param[in] dx x in the film
  * @param[in] dy y in the film
  * @param[in] sample_num the num of samples per pixel
  * @return the sample points
  */
  std::vector<vec2> samplePixel(int dx, int dy, int sample_num = PIXSAMPLE_NUM);

  
  /**
   * do camera look at
   * @param[in] lookAt look-at point
   * @param[in] refUp reference up vector
   */
  void lookAt(const vec3 &lookAt, const vec3 &refUp);
  /**
   * generate a ray in the world space with given film sample (dx, dy)
   * @param[in] dx x in the film
   * @param[in] dy y in the film
   */
  [[nodiscard]] Ray generateRay(Float dx, Float dy) const;
  /**
   * set a pixel's value
   * @param[in] dx x in the film
   * @param[in] dy y in the film
   * @param[in] value the value of the pixel
   */
  void setPixel(int dx, int dy, const vec3 &value);
  
  //update the pixel value after a round
  void updatePixel(int dx, int dy, const vec3 & value);
  /**
   * get the film of the camera
   * @return the film
   */
  vec3 getPixel(int dx,int dy);
  [[nodiscard]] const Film &getFilm() const;

 private:
  vec3 position;      // camera's position
  vec3 forward;       // forward vector
  vec3 right;         // right vector
  vec3 up;            // up vector
  Float verticalFov;  // vertical filed of view
  Float focalLength;  // focal length
  Film film;          // film
};

/**
 * make a perspective camera
 * @param[in] pos camera's position
 * @param[in] vFov vertical fov
 * @param[in] resolution film resolution
 * @return a shared_ptr of constructed camera
 */
std::shared_ptr<Camera> makeCamera(const vec3 &pos, Float vFov,
                                   const vec2i &resolution, Float focalLen = 1);

#endif  // CS171_HW3_INCLUDE_CAMERA_H_
