#pragma once
#include "Mesh.hpp"
#include "Model.hpp"
#include "GeomUtil.hpp"

/// <summary>
/// An AABBMesh is a regular triangle mesh, but intersection is accelerated
/// by first testing all the incoming rays against the smallest Axis Aligned Bounding Box
/// around the mesh.
/// This means most rays that don't pass through the mesh can be rejected.
/// </summary>
class AABBMesh : public Mesh
{
private:
	Eigen::Vector3f min_, max_;
public:
	AABBMesh(const Shader* shader, const Model* model, bool culling=true, IntersectMask mask=DEFAULT_BITMASK)
		:Mesh(shader, model, culling, mask)
	{
		modelToWorld(Eigen::Matrix4f::Identity());
	}

    virtual bool intersect(const Ray& ray, float minT, float maxT, HitInfo& info, IntersectMask mask) const override
    {
		if (!checkMask(mask)) return false;

		// Quick check for intersection with AABB.
		float minTtmp = minT, maxTtmp = maxT;
		for (int a = 0; a < 3; a++) {
			auto invD = 1 / ray.direction[a];
			auto orig = ray.origin[a];

			auto t0 = (min_[a] - orig) * invD;
			auto t1 = (max_[a] - orig) * invD;

			if (invD < 0)
				std::swap(t0, t1);

			if (t0 > minTtmp) minTtmp = t0;
			if (t1 < maxTtmp) maxTtmp = t1;

			if (maxTtmp <= minTtmp)
				return false;
		}

		// If we intersected the AABB, need to test the mesh.
		return Mesh::intersect(ray, minT, maxT, info, mask);
    }

	virtual void modelToWorld(const Eigen::Matrix4f& m) override
	{
		Entity::modelToWorld(m);

		// When changing modelToWorld, also update the world-space AABB.
		for (int i = 0; i < 3; ++i) {
			min_[i] = std::numeric_limits<float>::max();
			max_[i] = std::numeric_limits<float>::min();
		}
		for (int f = 0; f < model_->nfaces(); ++f) {
			for (int v = 0; v < 3; ++v) {
				Eigen::Vector3f v0 = model_->vert(model_->face(f)[v]);
				v0 = transformPosition(Entity::modelToWorld(), v0);
				for (int i = 0; i < 3; ++i) {
					if (v0[i] < min_[i]) min_[i] = v0[i];
					if (v0[i] > max_[i]) max_[i] = v0[i];
				}
			}
		}
	}

};

