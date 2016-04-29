#pragma once
#include "GameState.h"
#include <vector>
class ActionState : public GameState {
public:
	bool update();
	void render(const GLuint &programID);
	ActionState();
	~ActionState(); 
private:
	std::vector<GameObject*> mGameObjects;
	int mNrOfObjercts;
};
