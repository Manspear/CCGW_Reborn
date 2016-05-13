#include "Frustum.h"

void Frustum::extractPlanes( const glm::mat4& mvp )
{
	// Left clipping plane
	/*planes[0].x = mvp.m[12] + mvp.m[0];
	planes[0].y = mvp.m[13] + mvp.m[1];
	planes[0].z = mvp.m[14] + mvp.m[2];
	planes[0].w = mvp.m[15] + mvp.m[3];

	// Right clipping plane
	planes[1].x = mvp.m[12] - mvp.m[0];
	planes[1].y = mvp.m[13] - mvp.m[1];
	planes[1].z = mvp.m[14] - mvp.m[2];
	planes[1].w = mvp.m[15] - mvp.m[3];

	// Top clipping plane
	planes[2].x = mvp.m[12] - mvp.m[4];
	planes[2].y = mvp.m[13] - mvp.m[5];
	planes[2].z = mvp.m[14] - mvp.m[6];
	planes[2].w = mvp.m[15] - mvp.m[7];

	// Bottom clipping plane
	planes[3].x = mvp.m[12] + mvp.m[4];
	planes[3].y = mvp.m[13] + mvp.m[5];
	planes[3].z = mvp.m[14] + mvp.m[6];
	planes[3].w = mvp.m[15] + mvp.m[7];

	// Near clipping plane
	planes[4].x = mvp.m[12] + mvp.m[8];
	planes[4].y = mvp.m[13] + mvp.m[9];
	planes[4].z = mvp.m[14] + mvp.m[10];
	planes[4].w = mvp.m[15] + mvp.m[11];

	// Far clipping plane
	planes[5].x = mvp.m[12] - mvp.m[8];
	planes[5].y = mvp.m[13] - mvp.m[9];
	planes[5].z = mvp.m[14] - mvp.m[10];
	planes[5].w = mvp.m[15] - mvp.m[11];*/

	//glm::mat4 mvp = glm::transpose( m );

	mPlanes[0].x = mvp[0][3] + mvp[0][0];
	mPlanes[0].y = mvp[1][3] + mvp[1][0];
	mPlanes[0].z = mvp[2][3] + mvp[2][0];
	mPlanes[0].w = mvp[3][3] + mvp[3][0];

	// Right clipping plane
	mPlanes[1].x = mvp[0][3] - mvp[0][0];
	mPlanes[1].y = mvp[1][3] - mvp[1][0];
	mPlanes[1].z = mvp[2][3] - mvp[2][0];
	mPlanes[1].w = mvp[3][3] - mvp[3][0];

	// Top clipping plane
	mPlanes[2].x = mvp[0][3] - mvp[0][1];
	mPlanes[2].y = mvp[1][3] - mvp[1][1];
	mPlanes[2].z = mvp[2][3] - mvp[2][1];
	mPlanes[2].w = mvp[3][3] - mvp[3][1];

	// Bottom clipping plane
	mPlanes[3].x = mvp[0][3] + mvp[0][1];
	mPlanes[3].y = mvp[1][3] + mvp[1][1];
	mPlanes[3].z = mvp[2][3] + mvp[2][1];
	mPlanes[3].w = mvp[3][3] + mvp[3][1];

	// Near clipping plane
	mPlanes[4].x = mvp[0][3] + mvp[0][2];
	mPlanes[4].y = mvp[1][3] + mvp[1][2];
	mPlanes[4].z = mvp[2][3] + mvp[2][2];
	mPlanes[4].w = mvp[3][3] + mvp[3][2];

	// Far clipping plane
	mPlanes[5].x = mvp[0][3] - mvp[0][2];
	mPlanes[5].y = mvp[1][3] - mvp[1][2];
	mPlanes[5].z = mvp[2][3] - mvp[2][2];
	mPlanes[5].w = mvp[3][3] - mvp[3][2];

	for( int i=0; i<6; i++ )
		mPlanes[i] = glm::normalize( mPlanes[i] );
}

int Frustum::intersect( const glm::vec3& sphereCenter, float radius ) const
{
	int result = INSIDE;

	for( int i=0; i<6 && result == INSIDE; i++ )
	{
		//glm::vec3 planeNormal( mPlanes[i] );
		glm::vec3 planeNormal( mPlanes[i].x, mPlanes[i].y, mPlanes[i].z );
		float dist = glm::dot( planeNormal, sphereCenter ) + mPlanes[i].w;

		if( dist < -radius )
			result = OUTSIDE;
	}

	return result;
}

int Frustum::intersect( const BoundingBox& aabb ) const
{
	glm::vec3 corners[8];
	aabb.getCorners( corners );

	int totalInside = 0;

	for( int i=0; i<6; i++ )
	{
		int inCount = 8;
		int ptIn = 1;

		glm::vec3 planeNormal( mPlanes[i].x, mPlanes[i].y, mPlanes[i].z );
		for( int j=0; j<8; j++ )
		{
			float dist = glm::dot( planeNormal, corners[j] ) + mPlanes[i].w;
			if( dist < 0.0f )
			{
				ptIn = 0;
				inCount--;
			}
		}

		if( inCount == 0 )
			return OUTSIDE;

		totalInside += ptIn;
	}

	if(totalInside == 6 )
		return INTERSECT;
	return INSIDE;
}

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}