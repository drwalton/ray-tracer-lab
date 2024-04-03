#pragma once
#include "Renderable.hpp"
#include "GeomUtil.hpp"
#include "Model.hpp"

/// <summary>
/// An Mesh is a regular triangle mesh. Intersections are found by testing all triangles in the
/// mesh, which is slow for larger meshes.
/// See AABBMesh for a faster alternative.
/// </summary>
class Mesh : public Renderable
{
protected:
	const Model* model_;
	bool culling_;
public:
	Mesh(const Shader* shader, const Model* model, bool culling=true, IntersectMask mask=DEFAULT_BITMASK)
		:Renderable(shader, mask), model_(model), culling_(culling)
	{}

	virtual bool intersect(const Ray& ray, float minT, float maxT, HitInfo& info, IntersectMask mask) const override
	{
		if (!checkMask(mask)) return false;

		float closestT = std::numeric_limits<float>::max();
		HitInfo hitInfo;

		for (int f = 0; f < model_->nfaces(); ++f) {
			if (model_->face(f).size() != 3) {
				throw std::runtime_error("Supplied model file does not have triangular faces!");
			}

			Eigen::Vector3f v0 = model_->vert(model_->face(f)[0]);
			Eigen::Vector3f v1 = model_->vert(model_->face(f)[1]);
			Eigen::Vector3f v2 = model_->vert(model_->face(f)[2]);


			// Intersection code from
			// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
			Eigen::Vector3f v0World = transformPosition(modelToWorld(), v0);
			Eigen::Vector3f v1World = transformPosition(modelToWorld(), v1);
			Eigen::Vector3f v2World = transformPosition(modelToWorld(), v2);

			Eigen::Vector3f v0v1 = v1World - v0World;
			Eigen::Vector3f v0v2 = v2World - v0World;
			Eigen::Vector3f pvec = ray.direction.cross(v0v2);
			float det = v0v1.dot(pvec);

			if (culling_) {
				// if the determinant is negative, the triangle is 'back facing'
				// if the determinant is close to 0, the ray misses the triangle
				if (det < 1e-6) continue;
			}
			else {
				// ray and triangle are parallel if det is close to 0
				if (fabs(det) < 1e-6) continue;
			}

			float invDet = 1 / det;

			Eigen::Vector3f tvec = ray.origin - v0World;
			float u = tvec.dot(pvec) * invDet;
			if (u < 0 || u > 1) continue;

			Eigen::Vector3f qvec = tvec.cross(v0v1);
			float v = ray.direction.dot(qvec) * invDet;
			if (v < 0 || u + v > 1) continue;

			float t = v0v2.dot(qvec) * invDet;

			if (t >= closestT) continue;

			if (t < minT || t > maxT) continue;

			info.hitT = t;
			info.inDirection = ray.direction;
			info.location = ray.origin + t * ray.direction;
			info.shader = shader();

			if (model_->hasNormals()) {
				Eigen::Vector3f vn0 = model_->vn(model_->nface(f)[0]);
				Eigen::Vector3f vn1 = model_->vn(model_->nface(f)[1]);
				Eigen::Vector3f vn2 = model_->vn(model_->nface(f)[2]);
				vn0 = transformNormal(modelToWorld(), vn0);
				vn1 = transformNormal(modelToWorld(), vn1);
				vn2 = transformNormal(modelToWorld(), vn2);
				info.normal = ((1 - (u + v)) * vn0 + u * vn1 + v * vn2).normalized();
			}
			else 
				info.normal = v0v1.cross(v0v2).normalized();

			Eigen::Vector2f vt0 = model_->vt(model_->tface(f)[0]);
			Eigen::Vector2f vt1 = model_->vt(model_->tface(f)[1]);
			Eigen::Vector2f vt2 = model_->vt(model_->tface(f)[2]);
			info.texCoords = (1 - (u + v)) * vt0 + u * vt1 + v * vt2;

			closestT = t;
		}

		if (closestT == std::numeric_limits<float>::max()) {
			return false;
		}

		return true;
	}
};

