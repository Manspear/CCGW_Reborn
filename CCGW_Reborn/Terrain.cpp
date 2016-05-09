#include "Terrain.h"

Terrain::Terrain() : GameObject() {
	float degree = glm::pi<float>()*0.5f;
	mWorld = glm::mat4(cos(degree) , 0, -sin(degree), 0,
						0,	1,0,0,
						sin(degree), 0, cos(degree),0,
						13,0,45,1);
}

Terrain::~Terrain() {

}