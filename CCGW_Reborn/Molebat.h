#pragma once

#include "Enemy.h"
#include "Player.h"

#define MOLEBAT_FAR_DISTANCE 5.0f
#define MOLEBAT_NEAR_DISTANCE 3.0f
#define MOLEBAT_HEIGHT 3.0f
#define MOLEBAT_ALLY_DISTANCE 1.5f

//Class for representing a Molebat
class Molebat : public Enemy
{
public:
	//Chases the player, stays off the ground and updates position.
	void update();

	void setGameData( GameData* data );

	Molebat& operator=( const Molebat& ref );
	Molebat( const Molebat& ref );
	Molebat();
	~Molebat();

private:
	GameData* pGameData;
	int mSin;
};