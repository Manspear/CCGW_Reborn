#pragma once

#include "Enemy.h"

#define MOLERATMAN_SPEED 0.05f

//Class for representing a Moleratman
class Moleratman : public Enemy
{
public:
	//Follows a path and updates position
	void update();

	Moleratman& operator=( const Moleratman& ref );
	Moleratman( const Moleratman& ref );
	Moleratman();
	~Moleratman();
};