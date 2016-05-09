#include "Terrain.h"

Terrain::Terrain() : GameObject() {
	float degree = glm::pi<float>()*1.5f;
	float scale = 16.2f / 15.0f;
	glm::mat4 rotatematrix = { cosf(degree)*scale, 0, -sinf(degree)*scale,0,
		0,1*scale,0,0,
		sinf(degree)*scale, 0, cosf(degree)*scale,0,
		15,0,50,1
	};

	mWorld = rotatematrix;
}

Terrain::~Terrain() {

}