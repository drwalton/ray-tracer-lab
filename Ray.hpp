#pragma once
#include <Eigen/Dense>

struct Ray
{
	Eigen::Vector3f origin, direction;
};

std::ostream& operator <<(std::ostream& str, const Ray& ray)
{
	str << "Origin: " << ray.origin << "\n" << "Direction: " << ray.direction;
	return str;
}

