#pragma once 
#include "Entity.hpp"
#include "Ray.hpp"
#include "HitInfo.hpp"
#include "Shader.hpp"
#include "BitMasks.hpp"

class Shader;

/// <summary>
/// A Renderable is a type of Entity which can be rendered. Since
/// we're writing a ray-tracer, this means a ray can intersect it.
/// Each Renderable should have a pointer to a Shader. 
/// For some entities (e.g. Scene) where a Shader is not used, this
/// can be set to nullptr.
/// Renderable is an Abstract Data Type (ADT) as it has a pure virtual function.
/// To make a Renderable subclass you can instantiate, you must implement
/// the intersect function.
/// </summary>
class Renderable : public Entity
{
	const Shader* shader_;
	const IntersectMask mask_;
public:
	Renderable(const Shader* shader, const IntersectMask mask=DEFAULT_BITMASK)
		:shader_(shader), mask_(mask)
	{}

	virtual bool intersect(const Ray& ray, float minT, float maxT, HitInfo& info, IntersectMask mask) const = 0;

	bool checkMask(IntersectMask mask) const
	{
		return mask_ & mask;
	}

	const Shader* shader() const
	{
		return shader_;
	}
};

