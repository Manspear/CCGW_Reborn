#pragma once

#include "Enemy.h"

#define MOLERATMAN_SPEED 4.f

//Class for representing a Moleratman
class Moleratman : public Enemy
{
public:
	//Follows a path and updates position
	void update(float dt, GameData* data);
	//void render( GLuint programID );
	void render( GLuint worldLocation, GLuint animationLocation );

	void imHit( float strength, glm::vec3 position );

	//void setSpeed(float newSpeed);

	Moleratman& operator=( const Moleratman& ref );
	Moleratman( const Moleratman& ref );
	Moleratman();
	~Moleratman();
	GameData* pGameData;

private:
	enum
	{
		ANIM_WALK = 1,
		ANIM_STAGGER,
		ANIM_TURN_RIGHT,
		ANIM_TURN_LEFT,
		ANIM_STAGGER_COPY,
	};
};