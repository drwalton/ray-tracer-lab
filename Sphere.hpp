#pragma once
#include "Renderable.hpp"
#include "GeomUtil.hpp"

/// <summary>
/// A Sphere is a perfect sphere Renderable.
/// Set the radius in the constructor, and control the position and orientation using
/// the modelToWorld matrix.
/// Orientation affects the UV coordinates of the sphere.
/// </summary>
class Sphere : public Renderable
{
private:
	float radius_;
public:
	Sphere(const Shader* shader, float radius, IntersectMask mask=DEFAULT_BITMASK)
		:Renderable(shader, mask), radius_(radius)
	{}

	virtual ~Sphere()
	{}

	virtual bool intersect(const Ray& ray, float minT, float maxT, HitInfo& info, IntersectMask mask) const override
	{
		if (!checkMask(mask)) return false;

		Eigen::Vector3f centreWorldSpace = transformPosition(modelToWorld(), Eigen::Vector3f::Zero());

		Eigen::Vector3f centreToOrigin = ray.origin - centreWorldSpace;

		// Quadratic equation coefficients
		float a = 1.0f;
		float b = 2 * centreToOrigin.dot(ray.direction);
		float c = centreToOrigin.dot(centreToOrigin) - radius_ * radius_;

		double discriminant = b * b - 4 * a * c;

		// No intersection at all. Note we don't worry about the tangent case here!
		if (discriminant < 1e-6f) return false;

		// Two intersections.
		discriminant = sqrtf(discriminant); // This is safe now we know discriminant > 0.f
		float t0 = (-b - discriminant) / (2.f * a);
		float t1 = (-b + discriminant) / (2.f * a);

		if (t0 > t1) std::swap(t0, t1);

		float t;

		if (t0 > maxT || t1 < minT) return false;
		else if (t0 < minT) {
			if (t1 < maxT) t = t1;
			else return false;
		}
		else t = t0;

		info.hitT = t;
		info.location = ray.origin + t * ray.direction;
		info.normal = (info.location - centreWorldSpace).normalized();
		info.inDirection = ray.direction;
		info.shader = shader();
		Eigen::Vector3f modelSpaceLoc = transformPosition(modelToWorld().inverse(), info.location);
		modelSpaceLoc = modelSpaceLoc.normalized();
		info.texCoords = Eigen::Vector2f((atan2f(modelSpaceLoc.x(), modelSpaceLoc.z()) + M_PI) / (2.f * M_PI), (asinf(modelSpaceLoc.y()) / M_PI) + 0.5f);
		return true;
	}
};

