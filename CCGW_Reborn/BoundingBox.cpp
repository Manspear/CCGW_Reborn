#include "BoundingBox.h"
#include <math.h>


const bool BoundingBox::intersect(const BoundingBox* subject) const {
	return !(
						(fabsf(this->center.x - subject->center.x)	>	this->hWidth + subject->hWidth)
			||			(fabsf(this->center.y - subject->center.y)	>	this->hHeight + subject->hHeight)
			||			(fabsf(this->center.z - subject->center.z)	>	this->hDepth + subject->hDepth)
		);
}
const bool BoundingBox::intersect(glm::vec3 acenter, float halfSide) const {
	bool result = !(
						(fabsf(this->center.x - acenter.x)	>	this->hWidth  +	 halfSide)
					||	(fabsf(this->center.y - acenter.y)	>	this->hHeight +	 halfSide)
					||	(fabsf(this->center.z - acenter.z)	>	this->hDepth  +	 halfSide)
		);
	return result;
}

const bool BoundingBox::intersect(glm::vec3 point) const {
	return ( point.x > center.x - hWidth && point.x < center.x + hWidth &&
			point.y > center.y - hHeight && point.y < center.y + hHeight &&
			point.z > center.z - hDepth && point.z < center.z + hDepth );
}

const bool BoundingBox::intersect(glm::vec3 start, glm::vec3 end) const {
	float tenter = 0.0f, texit = 1.0f;

	if( !raySlabIntersect( center.x-hWidth, center.x+hWidth, start.x, end.x, tenter, texit ) )
		return false;

	if( !raySlabIntersect( center.y-hHeight, center.y+hHeight, start.y, end.y, tenter, texit ) )
		return false;

	if( !raySlabIntersect(center.z-hDepth, center.z+hDepth, start.z, end.z, tenter, texit ) )
		return false;

	return ( tenter < texit );
}

void BoundingBox::getCorners( glm::vec3* corners ) const
{
	// bottom 4
	corners[0] = center + glm::vec3( -hWidth, -hHeight, -hDepth );
	corners[1] = center + glm::vec3( hWidth, -hHeight, -hDepth );
	corners[2] = center + glm::vec3( hWidth, -hHeight, hDepth );
	corners[3] = center + glm::vec3( -hWidth, -hHeight, hDepth );

	// top 4
	corners[4] = center + glm::vec3( hWidth, hHeight, hDepth );
	corners[5] = center + glm::vec3( -hWidth, hHeight, hDepth );
	corners[6] = center + glm::vec3( -hWidth, hHeight, -hDepth );
	corners[7] = center + glm::vec3( hWidth, hHeight, -hDepth );
}

bool BoundingBox::raySlabIntersect( float slabmin, float slabmax, float raystart, float rayend, float& tbenter, float& tbexit ) const
{
	float raydir = rayend - raystart;

	if( fabs(raydir) < 1.0E-9f )
	{
		if( raystart < slabmin || raystart > slabmax )
			return false;
		return true;
	}

	float tsenter = ( slabmin - raystart ) / raydir;
	float tsexit = ( slabmax - raystart ) / raydir;

	if( tsenter > tsexit )
	{
		// TODO: Make swap macro
		float temp = tsenter;
		tsenter = tsexit;
		tsexit = temp;
	}

	if( tbenter > tsexit || tsenter > tsexit )
		return false;

	tbenter = ( tbenter > tsenter ? tbenter : tsenter );
	tbexit = ( tbexit < tsexit ? tbexit : tsexit );
	return true;
}

BoundingBox::BoundingBox(glm::vec3 centerPoint, float sides) {
	this->center = centerPoint;
	this->hDepth = sides/2;
	this->hHeight = sides/2;
	this->hWidth = sides/2;
}
BoundingBox::BoundingBox() {

}
BoundingBox::~BoundingBox() {

}