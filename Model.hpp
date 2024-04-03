#pragma once

#include <vector>
#include <Eigen/Dense>

/// <summary>
/// A Model stores mesh data and can load this data from an obj file.
/// </summary>
class Model {
private:
	std::vector<Eigen::Vector3f> verts_, vns_; // Vertices and vertex normals
	std::vector<Eigen::Vector2f> vts_; // Texture coordinates
	std::vector<std::vector<int> > faces_;  // Face indices of the vertices
	std::vector<std::vector<int> > tfaces_;  // Face indices of the texture coordinates
	std::vector<std::vector<int> > nfaces_;  // Face indices of the vertex normals
public:
	Model(const char *filename);
	~Model();
	int nverts() const;
	int nfaces() const;
	Eigen::Vector3f vert(int i) const;
	Eigen::Vector2f vt(int i) const;
	Eigen::Vector3f vn(int i) const;
	std::vector<int> face(int idx) const;
	std::vector<int> tface(int idx) const;
	std::vector<int> nface(int idx) const;
	bool hasNormals() const;
};

