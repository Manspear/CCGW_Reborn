#pragma once
#include "glm\vec3.hpp"

class BoundingBox {
public:
	glm::vec3 center;
	float hWidth, hHeight, hDepth;

	const bool intersect(const BoundingBox* subject) const;
	const bool intersect(glm::vec3 corner, float sides) const;
	const bool intersect(glm::vec3 point) const;
	const bool intersect(glm::vec3 start, glm::vec3 end) const;

	void getCorners( glm::vec3* corners ) const;

	BoundingBox(glm::vec3 centerPoint, float sides);
	BoundingBox();
	~BoundingBox();

private:
	bool raySlabIntersect( float slabmin, float slabmax, float raystart, float rayend, float& tbenter, float& tbexit ) const;
};