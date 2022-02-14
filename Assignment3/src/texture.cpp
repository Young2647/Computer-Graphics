#include <texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture()
{

}

Texture::Texture(const std::string &path)
{
	loadTextureFromFile(data, path, width, height, channel_num);
	if (channel_num != 3) 
	{
		std::cout << "not rgb texture!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < data.size(); i += channel_num)
	{
		Float R = (Float)data[i]/(Float)255;
		Float G = (Float)data[i+1]/(Float)255;
		Float B = (Float)data[i+2]/(Float)255;
		color_data.push_back(vec3(R,G,B));
	}
}

void Texture::loadNormaltexture(const std::string& path)
{
	loadTextureFromFile(data, path, normal_width, normal_height, normal_channel_num);
	if (normal_channel_num != 3)
	{
		std::cout << "not rgb texture!" << std::endl;
		exit(-1);
	}
	for (int i = 0; i < data.size(); i += normal_channel_num)
	{
		Float R = (Float)data[i] / (Float)255 * 2 - 1;
		Float G = (Float)data[i + 1] / (Float)255 * 2 - 1;
		Float B = (Float)data[i + 2] / (Float)255 * 2 - 1;
		normal_data.push_back(vec3(R, G, B).normalized());
	}
}

vec3 Texture::getColor(vec2 uv) const
{
	int x = (int) (uv.x() * width);
	int y = (int) (uv.y() * height);
	x = (x == width) ? x - 1 : x;
	y = (y == height) ? y - 1 : y;
	return color_data[x + y * width];
}

vec3 Texture::getTextnormal(vec2 uv) const
{
	int x = (int)(uv.x() * normal_width);
	int y = (int)(uv.y() * normal_height);
	x = (x == normal_width) ? x - 1 : x;
	y = (y == normal_height) ? y - 1 : y;
	return normal_data[x + y * normal_width];
}

void loadTextureFromFile(std::vector<std::uint8_t> &data,
                         const std::string &path, int &w, int &h, int &ch) {
	stbi_uc *img = stbi_load(path.c_str(), &w, &h, &ch, 0);
	if (!img)
	{
		std::cout << "can't load texture!" << std::endl;
		exit(-1);
	}
	data.assign(img, img + w * h * ch);
	stbi_image_free(img);
}