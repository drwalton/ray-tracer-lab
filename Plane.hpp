#pragma once
#include "Renderable.hpp"
#include "GeomUtil.hpp"

/// <summary>
/// The Plane consists of an infinite 2D plane defined by a normal vector.
/// To translate the plane, use the modelToWorld matrix.
/// </summary>
class Plane : public Renderable
{
private:
	Eigen::Vector3f normal_;
public:
	Plane(const Shader* shader, const Eigen::Vector3f& normal, IntersectMask mask=DEFAULT_BITMASK)
		:Renderable(shader, mask), normal_(normal)
	{}

	virtual ~Plane()
	{}

	virtual bool intersect(const Ray& ray, float minT, float maxT, HitInfo& info, IntersectMask mask) const override
	{
		if (!checkMask(mask)) return false;

		// Work out plane position and normal in world space.
		Eigen::Vector3f centreWorldSpace = transformPosition(modelToWorld(), Eigen::Vector3f::Zero());
		Eigen::Vector3f normalWorldSpace = transformNormal(modelToWorld(), normal_);

		float rayDotNorm = ray.direction.dot(normalWorldSpace);
		if (abs(rayDotNorm) < 1e-6f) return false; // ray parallel to plane.

		float t = (centreWorldSpace - ray.origin).dot(normalWorldSpace) / rayDotNorm;
		if (t < minT || t > maxT) return false; // intersection not in range.

		info.hitT = t;
		info.inDirection = ray.direction;
		info.location = ray.origin + t * ray.direction;
		info.normal = normalWorldSpace;
		info.shader = shader();
		info.texCoords = Eigen::Vector2f(
			fmodf(info.location.x(), 1.0f),
			fmodf(info.location.y(), 1.0f));
		
		return true;
	}
};

