#pragma once

#include "Particle.h"
#include "Billboard.h"

//Class for emitting particles. Don't instantiate this directly.
class Emitter
{
	//Let the Emission-class know about this classes private members.
	friend class Emission;

public:
	//Load the specified texture for all the particles in this Emitter.
	bool load( GameData* data, std::string texture );
	//Spawn a new particle.
	void spawn( glm::vec3 position, glm::vec3 velocity, float lifetime, float drag = 0.5f, glm::vec2 startScale = glm::vec2(1.0f), glm::vec2 endScale = glm::vec2(1.0f) );

	Emitter();
	~Emitter();

private:
	Particle* pParticles;
	int mMax;
};

//Class for allocating,updating and rendering particles. Use this class to instantiate Emitters.
class Emission
{
public:
	//Allocates a specified number of particles. Returns an Emitter that can spawn said particles.
	bool allocEmitter( Emitter* emitter, int maxParticles );
	//Update all the particles.
	void update( float deltaTime );
	//Draw all the particles.
	void draw();

	Emission( GameData* data, int maxParticles );
	~Emission();

private:
	//Sorts the particles back to front based on the distance to the camera. Particles further back are put at the end of the sorted array.
	void sort();

	Camera* pCamera;
	BillboardProgram* pBillboardProgram;

	Particle* mpParticles, **mpSorted;
	int mMax, mSize, mAlive;
	float* mDistances;
};