#pragma once

#include <glm\glm.hpp>
#include "BoundingBox.h"

class Frustum
{
public:
	enum
	{
		OUTSIDE = 0,
		INSIDE,
		INTERSECT,
	};

	void extractPlanes( const glm::mat4& mvp );
	int intersect( const glm::vec3& sphereCenter, float radius ) const;
	int intersect( const BoundingBox& aabb ) const;

	Frustum();
	~Frustum();

private:
	glm::vec4 mPlanes[6];
};