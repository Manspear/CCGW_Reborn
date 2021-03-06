#pragma once

#include "Enemy.h"
#include "Player.h"

#define MOLEBAT_FAR_DISTANCE 4.5f
#define MOLEBAT_NEAR_DISTANCE 2.5f
#define MOLEBAT_HEIGHT 3.5f
#define MOLEBAT_ALLY_DISTANCE 1.5f

//Class for representing a Molebat
class Molebat : public Enemy
{
public:
	//Chases the player, stays off the ground and updates position.
	void update(float dt);
	//void render( GLuint programID );
	void render( GLuint worldLocation, GLuint animationLocation );

	void imHit( float strength, glm::vec3 position );

	void setGameData( GameData* data );
	//void setSpeed(float newSpeed);
	Molebat& operator=( const Molebat& ref );
	Molebat( const Molebat& ref );
	Molebat();
	~Molebat();

	

private:
	GameData* pGameData;
	int mSin;
	float mTimeSinceLastHit;
	//float mSpeed;
};