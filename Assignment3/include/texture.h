#ifndef CS171_HW3_INCLUDE_TEXTURE_H_
#define CS171_HW3_INCLUDE_TEXTURE_H_
#include <vector>
#include <cstdint>
#include <string>
#include <core.h>

// optional
// you can change the structure if you want
class Texture {
public :	
	Texture(const std::string & path);
	Texture();
	vec3 getColor(vec2 uv) const;
	vec3 getTextnormal(vec2 uv) const;
	void loadNormaltexture(const std::string& path);
	bool HaveNormaltexture() const { return !normal_data.empty(); }
private:
	std::vector<std::uint8_t> data;
	std::vector<vec3> color_data;
	std::vector<vec3> normal_data;
	int width;
	int height;
	int channel_num;
	int normal_width;
	int normal_height;
	int normal_channel_num;
};

void loadTextureFromFile(std::vector<std::uint8_t> &data,
                         const std::string &path, int &w, int &h, int &ch);

#endif