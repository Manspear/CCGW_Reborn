#pragma once
#include "GameObject.h"


class Marker : public GameObject {
public:
	void update(const Player* gameData);
	bool update(const Input * inputs, GameData &gameData);
	virtual void render(const GLuint &programID);
	std::vector<glm::vec2> getMarkedTiles();
	void resetMarkedTiles();
	Marker();
	~Marker();
private:
	std::vector<glm::vec2> mMarkedIndex;
	glm::vec2 selectedTile;
	bool mPicked;
	glm::vec2 mousePicking(const glm::vec2 mousePos, const GameData &gameData);
};