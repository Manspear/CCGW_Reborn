#include "ActionState.h"

bool ActionState::update() {
	for (int i = 0; i < mGameObjects.size(); i++) {
		mGameObjects[i]->update(0.0);
	}
	return true;
}

void ActionState::render(const GLuint & programID)
{
	for (int i = 0; i < mGameObjects.size(); i++) {
		mGameObjects[i]->render(programID, glm::mat4());
	}
}

ActionState::ActionState(): mNrOfObjercts(0) {

}

ActionState::~ActionState() {

}