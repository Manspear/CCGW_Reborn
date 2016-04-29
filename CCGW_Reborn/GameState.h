#pragma once
#include "GameObject.h"
class GameState {
public:
	virtual bool update() = 0;
	virtual void render(const GLuint &programID) = 0;

	GameState() {};
	virtual ~GameState() {};
	
};