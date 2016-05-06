#include "Terrain.h"

Terrain::Terrain() : GameObject() {
	float degree = glm::pi<float>()*1.5f;
	glm::mat4 rotatematrix = { cosf(degree), 0, -sinf(degree),0,
		0,1,0,0,
		sinf(degree), 0, cosf(degree),0,
		14,0,46,1
	};

	mWorld = rotatematrix;
}

Terrain::~Terrain() {

}