#include <brdf.h>
#include <utils.h>
#include <interaction.h>

/**
 * IdealDiffusion class
 */

IdealDiffusion::IdealDiffusion(vec3 diff) : reflectivity(diff) {}

vec3 IdealDiffusion::eval(const Interaction& interact) {
	//return reflectivity;
	return pdf(interact) * reflectivity;
}

Float IdealDiffusion::pdf(const Interaction& interact) {
	return 1.0f;
}

Float IdealDiffusion::sample(Interaction& interact) {
	auto random_samples = unif(0.0, 1.0, 2);

	//sampling on a disk
	Float x1 = random_samples[0];
	Float y1 = random_samples[1];

	//cosine-weighted hemisphere sampling
	Float sin_theta = sqrt(x1);
	Float cos_theta = sqrt(1 - x1);
	Float phi = 2 * PI * y1;
	vec3 tmp_wi = vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
	Float theta = acos(interact.normal.dot(vec3(0, 0, 1)));
	vec3 rotation_axis = vec3(0,0,1).cross(interact.normal);
	Eigen::AngleAxisf rotationVector(theta, rotation_axis);
	Eigen::Matrix3f rotation_matrix = rotationVector.toRotationMatrix();
	interact.wi = -(rotation_matrix * tmp_wi).normalized(); // from (0,0,1) system to world coordinate
	return cos_theta / PI; //
}

bool IdealDiffusion::isDelta() const { return false; }


//Ideal Specular
IdealSpecular::IdealSpecular(vec3 spec) : reflectivity(spec) {}


vec3
IdealSpecular::eval(const Interaction& interact)
{
	return pdf(interact) * reflectivity;
}

Float
IdealSpecular::pdf(const Interaction& interact)
{
	vec3 reflect = (2 * interact.wo.dot(interact.normal) * interact.normal - interact.wo).normalized();
	if ((reflect + interact.wi).norm() < DEFAULT_DELTA)
	{
		return 1.0f;
	}
	else return 0.0f;
}

Float
IdealSpecular::sample(Interaction& interact)
{
	interact.wi = (interact.wo - 2 * interact.wo.dot(interact.normal) * interact.normal).normalized();
	return 1.0f; // pdf = 1.0 because it always reflect in one dir.
}

bool
IdealSpecular::isDelta() const { return true; }



//Transmission
IdealTransmission::IdealTransmission(vec3 reflect, Float ior) : reflectivity(reflect)
{
	this->ior = ior;
}

vec3
IdealTransmission::eval(const Interaction& interact)
{
	return pdf(interact) * reflectivity;
}

Float
IdealTransmission::pdf(const Interaction& interact)
{
	Float cos_theta = interact.wo.dot(interact.normal);
	vec3 refract = vec3::Zero();
	if (cos_theta < 0)
	{
		cos_theta = -cos_theta;
		Float sin_theta_t = sqrt(ior * ior * (1 - cos_theta * cos_theta));
		if (1 - sin_theta_t * sin_theta_t < 0)
			refract = (interact.wo - 2 * interact.wo.dot(-interact.normal) * -interact.normal).normalized();
		else
			refract = -(ior * (-interact.wo) + (ior * cos_theta - sqrt(1 - sin_theta_t * sin_theta_t)) * (-interact.normal));
	}
	else
	{
		Float sin_theta_t = sqrt((1 / ior) * (1 / ior) * (1 - cos_theta * cos_theta));
		if (1 - sin_theta_t * sin_theta_t < 0)
		{
			refract = (interact.wo - 2 * interact.wo.dot(interact.normal) * interact.normal).normalized();
		}
		else
		{
			refract = -((1 / ior) * (-interact.wo) + ((1 / ior) * cos_theta - sqrt(1 - sin_theta_t * sin_theta_t)) * interact.normal);
		}
	}
	refract = refract.normalized();
	if ((interact.wi - refract).norm() < DEFAULT_DELTA)
	{
		return 1.0f;
	}
	else
	{
		return 0.0f;
	}
}

Float
IdealTransmission::sample(Interaction& interact)
{
	Float cos_theta = interact.wo.dot(interact.normal);
	if (cos_theta < 0)
	{
		cos_theta = -cos_theta;
		Float sin_theta_t = sqrt(ior * ior * (1 - cos_theta * cos_theta));
		if (1 - sin_theta_t * sin_theta_t < 0)
			interact.wi = (interact.wo - 2 * interact.wo.dot(-interact.normal) * -interact.normal).normalized();
		else
			interact.wi = -(ior * (-interact.wo) + (ior * cos_theta - sqrt(1 - sin_theta_t * sin_theta_t)) * (-interact.normal));
	}
	else
	{
		Float sin_theta_t = sqrt((1 / ior) * (1 / ior) * (1 - cos_theta * cos_theta));
		if (1 - sin_theta_t * sin_theta_t < 0)
		{
			interact.wi = (interact.wo - 2 * interact.wo.dot(interact.normal) * interact.normal).normalized();
		}
		else
		{
			interact.wi = -((1 / ior) * (-interact.wo) + ((1 / ior) * cos_theta - sqrt(1 - sin_theta_t * sin_theta_t)) * interact.normal);
		}
	}
	interact.wi = interact.wi.normalized();
	return 1.0f;
}

bool
IdealTransmission::isDelta() const { return true; }



GlossyMetal::GlossyMetal(vec3 reflect, Float roughness) : reflectivity(reflect)
{
	this->roughness = roughness;
}

vec3
GlossyMetal::eval(const Interaction& interact)
{
	vec3 H = (interact.wo - interact.wi).normalized();
	Float cos_theta_h = H.dot(interact.normal);
	Float cos_theta_l = (-interact.wi).dot(interact.normal);
	Float cos_theta_v = interact.wo.dot(interact.normal);
	Float cos_theta_d = interact.wo.dot(H);
	Float alpha = roughness * roughness;
	Float t = 1 + ((alpha * alpha) - 1) * cos_theta_h * cos_theta_h;
	Float GTR = alpha * alpha / (PI * t * t); // glossy specular function
	Float F = 0.08 + (1-0.08)* std::pow(1-(-interact.wi).dot(H), 8);
	Float G = 1 / (cos_theta_l + sqrt(alpha + cos_theta_l * cos_theta_l - alpha * cos_theta_l * cos_theta_l));
	G *= 1 / (cos_theta_v + sqrt(alpha + cos_theta_v * cos_theta_v - alpha * cos_theta_v * cos_theta_v));
	G = std::min(G, 1.0f);
	Float Spec = GTR * F * G; // specular coefficient

	//Float Fd90 = 0.5 + 2 * (-interact.wi).dot(H) * (-interact.wi).dot(H) * roughness * roughness;
	//Float diffuse = (1 + (Fd90 - 1) * std::pow(1 - cos_theta_l, 5)) * (1 + (Fd90 - 1) * std::pow(1 - cos_theta_v, 5)); // glossy diffuse function
	//diffuse = std::min(diffuse, 1.0f);
	return Spec * reflectivity;
}

Float
GlossyMetal::pdf(const Interaction& interact)
{
	vec3 H = (interact.wo - interact.wi).normalized();
	Float cos_theta_h = H.dot(interact.normal);
	Float cos_theta_l = (-interact.wi).dot(interact.normal);
	Float cos_theta_v = interact.wo.dot(interact.normal);
	Float cos_theta_d = interact.wo.dot(H);
	Float alpha = roughness * roughness;
	Float t = 1 + ((alpha * alpha) - 1) * cos_theta_h * cos_theta_h;
	Float GTR = alpha * alpha / (PI * t * t); // glossy specular function
	Float Spec = std::min(GTR, 1.0f); // specular coefficient

	Float pdf_h = GTR * cos_theta_h;
	//Float pdf_l = cos_theta_h/PI;
	Float pdf_l = pdf_h / (4 * (-interact.wi).dot(H));
	return pdf_l;
}

Float
GlossyMetal::sample(Interaction& interact)
{
	auto random_samples = unif(0.0, 1.0, 2);

	//sampling on a disk
	Float x1 = random_samples[0];
	Float y1 = random_samples[1];

	//BRDF weighted sampling
	Float phi = 2 * PI * x1;
	Float alpha = roughness * roughness;
	Float cos_theta = sqrt((1 - y1) / (1 + (alpha * alpha - 1) * y1));
	Float sin_theta = sqrt(std::max(0.0f, 1 - cos_theta * cos_theta));
	//vec3 tmp_H = vec3(0, 0, 1);
	vec3 tmp_H = vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
	//Float theta = acos(interact.normal.dot(vec3(0, 0, 1)));
	//vec3 rotation_axis = vec3(0, 0, 1).cross(interact.normal);
	//Eigen::AngleAxisf rotationVector(theta, rotation_axis);
	//Eigen::Matrix3f rotation_matrix = rotationVector.toRotationMatrix();
	Eigen::Matrix3f rotation_matrix = Eigen::Quaternionf::FromTwoVectors(vec3(0, 0, 1), interact.normal).toRotationMatrix();
	vec3 H = (rotation_matrix * tmp_H).normalized(); // from (0,0,1) system to world coordinate
	interact.wi = (interact.wo - 2 * interact.wo.dot(H) * H).normalized();
	//interact.wi = (interact.wo - 2 * interact.wo.dot(interact.normal) * interact.normal).normalized();
	return pdf(interact);
}

bool
GlossyMetal::isDelta() const { return true; }

std::shared_ptr<BRDF> makeIdealDiffusion(const vec3 &diff) {
  return std::make_shared<IdealDiffusion>(diff);
}

std::shared_ptr<BRDF> makeIdealSpecular(const vec3& spec) {
	return std::make_shared<IdealSpecular>(spec);
}

std::shared_ptr<BRDF> makeIdealTransmission(const vec3& reflect, const Float ior) {
	return std::make_shared<IdealTransmission>(reflect, ior);
}

std::shared_ptr<BRDF> makeGlossyMetal(const vec3& reflect, const Float roughness) {
	return std::make_shared<GlossyMetal>(reflect, roughness);
}