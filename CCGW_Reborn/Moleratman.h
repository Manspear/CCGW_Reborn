#pragma once

#include "Enemy.h"

#define MOLERATMAN_SPEED 1.f

//Class for representing a Moleratman
class Moleratman : public Enemy
{
public:
	//Follows a path and updates position
	void update(float dt);
	void render( GLuint programID );

	Moleratman& operator=( const Moleratman& ref );
	Moleratman( const Moleratman& ref );
	Moleratman();
	~Moleratman();
	GameData* pGameData;
};