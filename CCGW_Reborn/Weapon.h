#pragma once
#include "GameObject.h"
#include "Arrow.h"
#include "Sound.h"

#define WEAPON_MAX_ARROWS 10

class Weapon 
{
public:
	bool load( GameData* data, bool playerOwned, Emitter* emitter );
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation);
	void shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float strength);
	void update(float dt);
	//void draw(const GLuint &programID);
	void render( GLuint worldLocation );
	float getStrength() const;
	float getRange() const;
	void loadSound(Sound* sound);
	
	Weapon();
	~Weapon();

private:
	bool mPlayerOwned;
	Arrow mpArrows[WEAPON_MAX_ARROWS];
	float mStrength;
	float mRange;
};
