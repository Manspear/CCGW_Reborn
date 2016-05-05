#pragma once
#include "GameObject.h"
#include "Arrow.h"

#define WEAPON_MAX_ARROWS 10

class Weapon 
{
public:
	bool load( GameData* data, bool playerOwned );
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation);
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float strength);
	void draw(const GLuint &programID);
	void update(float dt);
	float getStrength() const;
	float getRange() const;
	
	//Weapon(bool owner, GameData* data);
	Weapon();
	~Weapon();

private:
	bool mPlayerOwned;
	Arrow mpArrows[WEAPON_MAX_ARROWS];
	float mStrength;
	//int mMax;
	float mRange;
};
