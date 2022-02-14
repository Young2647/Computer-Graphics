#include <film.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Film::Film(const vec2i &resolution) : resolution(resolution) {
  this->pixels.resize(resolution.x() * resolution.y());
}

void Film::write(const std::string &filePath) const {
  std::vector<std::uint8_t> data;
  data.reserve(resolution.x() * resolution.y() * 3);

  auto gammaCorrection = [](Float x) {
    if (x > 1) x = 1;
    if (x < 0) x = 0;
    return (std::uint8_t)(pow(x, 1 / 2.2) * 255);
  };

  for (auto &v : pixels) {
    for (int i = 0; i < 3; i++) {
      data.push_back(gammaCorrection(v[i]));
    }
  }
  stbi_flip_vertically_on_write(true);
  stbi_write_png(filePath.c_str(), resolution.x(), resolution.y(), 3,
                 data.data(), 0);
}
