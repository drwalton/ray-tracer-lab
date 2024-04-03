#pragma once
#include <Eigen/Dense>

class Shader;

struct HitInfo
{
	float hitT;
	Eigen::Vector3f normal, location, inDirection;
	const Shader* shader;
	Eigen::Vector2f texCoords;
};
