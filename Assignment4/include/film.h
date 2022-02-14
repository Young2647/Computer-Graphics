#ifndef CS171_HW3_INCLUDE_FILM_H_
#define CS171_HW3_INCLUDE_FILM_H_
#include <core.h>
#include <vector>
/**
 * Film class
 */
struct Film {
  /**
   * construct the film with given resolution
   * @param[in] resolution the resolution of the film
   */
  explicit Film(const vec2i &resolution);

  /**
   * get the aspect ratio of the film
   * @return aspect ratio
   */
  [[nodiscard]] Float getAspectRatio() const {
    return static_cast<Float>(resolution.x()) /
           static_cast<Float>(resolution.y());
  }
  /**
   * write film to image (write to png file)
   * @param[in] filePath the path to wrtie to (ends with .png)
   */
  void write(const std::string &filePath) const;

  const vec2i resolution;    // resolution of the film
  std::vector<vec3> pixels;  // pixels data
};

#endif  // CS171_HW3_INCLUDE_FILM_H_
