#pragma once
#include "GameObject.h"
#include "Arrow.h"


class Weapon 
{
public:
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation);
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float strength);
	void draw(const GLuint &programID);
	void update(float dt);
	float getStrength() const;
	float getRange() const;
	Weapon(bool owner, GameData* data);
	~Weapon();

private:
	bool playerOwned;
	Arrow *arrows;
	float mStrength;
	int mMax;
	float mRange;
};
