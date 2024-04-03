#pragma once

#include <vector>
#include <Eigen/Dense>

class Model {
private:
	std::vector<Eigen::Vector3f> verts_, vns_;              // Stores Vec3f for every model vertex world position
	std::vector<std::vector<int> > faces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<std::vector<int> > tfaces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<std::vector<int> > nfaces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<Eigen::Vector2f> vts_;				// Stores Vec3f for every model vertex texture coordinate
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

