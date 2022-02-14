#include <material.h>
#include <interaction.h>

/**
 * ColorMaterial class
 */

ConstantMaterial::ConstantMaterial()
    : diffuse(vec3(1.0, 1.0, 1.0)), specular(vec3(0, 0, 0)), shininess(0.0) {}

ConstantMaterial::ConstantMaterial(vec3 diff, vec3 spec, vec3 amb, Float sh)
    : diffuse(diff), specular(spec), ambient(amb), shininess(sh) {}

ConstantMaterial::ConstantMaterial(vec3 rgb, Float sh = 16.0f)
    : diffuse(rgb), specular(rgb), ambient(rgb / 10), shininess(sh) {}

InteractionPhongModel ConstantMaterial::evaluate(const Interaction &in) const {
  InteractionPhongModel m;
  m.diffusion = diffuse;
  m.specular = specular;
  m.ambient = ambient;
  m.shininess = shininess;

  return m;
}

vec3 ConstantMaterial::evalTextnormal(const Interaction &in, Eigen::Matrix3f tbn) const
{
	return in.normal;
}


TextureMaterial::TextureMaterial() : shininess(0.0) {}

TextureMaterial::TextureMaterial(const std::string &path, Float shininess = 16.0f)
{
	texture = Texture(path);
	this->shininess = shininess;
}

InteractionPhongModel TextureMaterial::evaluate(const Interaction &in) const {
	InteractionPhongModel m;

	m.ambient = texture.getColor(in.uv)/10;
	m.diffusion = texture.getColor(in.uv);
	m.specular = texture.getColor(in.uv);
	
	m.shininess = shininess;

	return m;
}

vec3 TextureMaterial::evalTextnormal(const Interaction& in, Eigen::Matrix3f tbn) const
{
	if (texture.HaveNormaltexture())
		//return texture.getTextnormal(in.uv);
		return (tbn * texture.getTextnormal(in.uv)).normalized();
	else
		return in.normal;
}

void TextureMaterial::loadNormaltexture(const std::string& path)
{
	texture.loadNormaltexture(path);
}

std::shared_ptr<ConstantMaterial> makeConstantMaterial(const vec3 &rgb,
                                                       Float shininess) {
  return std::make_shared<ConstantMaterial>(rgb, shininess);
}

std::shared_ptr<TextureMaterial> makeTextureMaterial(const std::string &path,
                                                       Float shininess) {
	return std::make_shared<TextureMaterial>(path, shininess);													   
}