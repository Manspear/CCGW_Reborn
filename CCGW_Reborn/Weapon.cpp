#include "Weapon.h"

void Weapon::shoot(glm::vec3 position, glm::vec3 lookat, float rotation) 
{
	bool shot = false;
	for (int i = 0; i < mMax && !shot; i++) {
		if (!arrows[i].isAlive()) {
			arrows[i].spawn(playerOwned,position, lookat,15*mStrength, { 0,-1,0 }, rotation);
			shot = true;
		}
	}
	this->mStrength = 5;
}
void Weapon::shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float strength)
{
	bool shot = false;
	for (int i = 0; i < mMax && !shot; i++) {
		if (!arrows[i].isAlive()) {
			arrows[i].spawn(playerOwned, position, lookat, 15 * strength, { 0,-1,0 }, rotation);
			shot = true;
		}
	}
	mStrength = strength;
}
void Weapon::update(float dt) {
	for (int i = 0; i < mMax; i++) {
		if(arrows[i].isAlive())
			arrows[i].update(dt);
	}
}

void Weapon::draw(const GLuint &programID) {
	for (int i = 0; i < mMax; i++) {
		if (arrows[i].isAlive())
			arrows[i].render(programID);
	}
}

float Weapon::getRange() const
{
	return mRange;
}

float Weapon::getStrength() const
{
	return mStrength;
}

Weapon::Weapon(bool owner, GameData* data)
{
	this->playerOwned = owner;
	Texture* tex = data->pAssets->load<Texture>("Models/pns.png");
	mMax = 100;
	this->arrows = new Arrow[mMax];
	for (int i = 0; i < mMax; i++)
	{
 		this->arrows[i].loadTex(tex);
		//this->arrows[i].load(data, "Models/threeCubes.mole");
		this->arrows[i].load( data, "Models/arrow_best3.obj"); // arrow_liten4_one	
	}
	this->mStrength = 5;
}

Weapon::~Weapon() 
{
	delete[] arrows;
}