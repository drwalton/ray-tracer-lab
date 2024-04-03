#pragma once
#include "Shader.hpp"
#include "tgaimage.h"

/// <summary>
/// Lambertian reflectance shader that samples albedo values from a texture.
/// The texture should be stored as an image file (TGAImage instance).
/// </summary>
class TexturedLambertianShader : public Shader
{
private:
	const TGAImage* albedoTexture_;
	bool shadowTest_;
public:
	TexturedLambertianShader(const TGAImage* albedoTexture, bool shadowTest=true)
		:shadowTest_(shadowTest), albedoTexture_(albedoTexture)
	{}

	virtual Eigen::Vector3f getColor(const HitInfo& hitInfo, 
		const Renderable* scene, 
		const std::vector<std::unique_ptr<Light>>& lights,
		const Eigen::Vector3f& ambientLight,
		int currBounceCount,
		const int maxBounces) const
	{
		Eigen::Vector3f albedo;

		Eigen::Vector2f tex = hitInfo.texCoords;
		int pixX = static_cast<int>(tex.x() * albedoTexture_->get_width());
		int pixY = static_cast<int>((1.f - tex.y()) * albedoTexture_->get_height());
		pixX = std::max(pixX, 0);
		pixY = std::max(pixY, 0);
		pixX = std::min(pixX, albedoTexture_->get_width());
		pixY = std::min(pixY, albedoTexture_->get_height());
		TGAColor albedoTGA = albedoTexture_->get(pixX, pixY);

		albedo.x() = static_cast<float>(albedoTGA.r) / 255.f;
		albedo.y() = static_cast<float>(albedoTGA.g) / 255.f;
		albedo.z() = static_cast<float>(albedoTGA.b) / 255.f;

		Eigen::Vector3f color = coefftWiseMul(albedo, ambientLight);

		for (auto& light : lights) {
			if (shadowTest_) {
				if (!light->visibilityCheck(hitInfo.location, scene))
					continue;
			}
			Eigen::Vector3f lightVec = light->getVecToLight(hitInfo.location);
			float dotProd = std::max(lightVec.dot(hitInfo.normal), 0.f);
			color += dotProd * coefftWiseMul(light->getIntensity(hitInfo.location), albedo);
		}

		return color;
	}
};

