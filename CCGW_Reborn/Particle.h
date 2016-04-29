#pragma once

#include <glm\glm.hpp>
#include "Billboard.h"
#include "Texture.h"

class Particle
{
public:
	/*Loads a texture for the particle.*/
	bool load( GameData* data, string texture );
	/*Spawns the particle at the supplied position.*/
	void spawn( glm::vec3 position, glm::vec3 velocity, float lifetime, float drag = 0.5f, glm::vec2 startScale = glm::vec2( 1.0f ), glm::vec2 endScale = glm::vec2( 1.0f ) );
	/*Updates the position, velocity, scale and lifetime of the particle.*/
	void update( const glm::vec3& cameraPosition, float deltaTime );
	/*Draws the particle.*/
	void draw( Camera* camera, BillboardProgram* billboardProgram );

	glm::vec3 getPosition() const;
	glm::vec3 getVelocity() const;
	glm::vec2 getScale() const;

	float getLifetime() const;
	float getElapsed() const;
	bool getShouldAdd();
	
	Particle& operator=( const Particle& ref );
	Particle( const Particle& ref );
	Particle();
	~Particle();

private:
	glm::vec3 mPosition;
	glm::vec3 mVelocity;
	glm::vec2 mScale;

	glm::vec2 mStartScale;
	glm::vec2 mEndScale;
	float mLifetime, mElapsed;
	float mDrag;
	bool mShouldAdd;

	Texture* mpTexture;
};