#ifndef CS171_HW3_INCLUDE_MATERIAL_H_
#define CS171_HW3_INCLUDE_MATERIAL_H_
#include <interaction.h>
#include <texture.h>
/**
 * Base class of materials
 */
class Material {
 public:
  Material() = default;
  virtual ~Material() = default;
  /**
   * evaluate phong model
   */
  [[nodiscard]] virtual InteractionPhongModel evaluate(
      const Interaction &in) const = 0;

  [[nodiscard]] virtual vec3 evalTextnormal(
      const Interaction& in, Eigen::Matrix3f tbn = Eigen::Matrix3f::Zero()) const = 0;
};

/**
 * Constant color-based materials
 */
class ConstantMaterial : public Material {
 protected:
  vec3 diffuse;
  vec3 specular;
  vec3 ambient;
  Float shininess;

 public:
  ConstantMaterial();
  ConstantMaterial(vec3 diff, vec3 spec, vec3 amb, Float sh = 16.0f);
  ConstantMaterial(vec3 rgb, Float shininess);
  [[nodiscard]] InteractionPhongModel evaluate(
      const Interaction &in) const override;

  [[nodiscard]] virtual vec3 evalTextnormal(
      const Interaction& in, Eigen::Matrix3f tbn = Eigen::Matrix3f::Zero()) const override;
};

// optional
class TextureMaterial : public Material {
protected :
    Texture texture;
	Float shininess;

public :
	TextureMaterial();
	TextureMaterial(const std::string &path, Float shininess);
	[[nodiscard]] InteractionPhongModel evaluate(
	const Interaction &in) const override;	

    [[nodiscard]] virtual vec3 evalTextnormal(
        const Interaction& in, Eigen::Matrix3f tbn = Eigen::Matrix3f::Zero()) const override;
    void loadNormaltexture(const std::string& path);
    // optional TODO: finish this
};

std::shared_ptr<ConstantMaterial> makeConstantMaterial(const vec3 &rgb,
                                                       Float shininess);

std::shared_ptr<TextureMaterial> makeTextureMaterial(const std::string &path,
                                                       Float shininess);													   
#endif  // CS171_HW3_INCLUDE_MATERIAL_H_
