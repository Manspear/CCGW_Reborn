#include "Weapon.h"

bool Weapon::load( GameData* data, bool playerOwned, Emitter* emitter )
{
	bool result = true;

	Model* arrowModel = data->pAssets->load<Model>( "Models/arrowpx.mole" );

	mPlayerOwned = playerOwned;
	for( int i=0; i<WEAPON_MAX_ARROWS && result; i++ )
		result = result && mpArrows[i].load( data, arrowModel, emitter );

	return result;
}

//The weapon of the towers.
void Weapon::shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float damage) 
{
	mStrength = 5;
	towerDamage = damage;
	shoot( position, lookat, rotation, mStrength,  damage);
}

void Weapon::shoot(glm::vec3 position, glm::vec3 lookat, float rotation, float strength, float damage)
{
	bool shot = false;
	for (int i = 0; i<WEAPON_MAX_ARROWS && !shot; i++)
	{
		if (!mpArrows[i].isAlive())
		{
			mpArrows[i].spawn(mPlayerOwned, position, lookat, 15 * strength, { 0, -1, 0 }, rotation, damage);
			shot = true;
		}
	}
	mStrength = damage;
}
void Weapon::update(float dt) {
	for( int i=0; i<WEAPON_MAX_ARROWS; i++ )
	{
		if( mpArrows[i].isAlive() )
		{
			mpArrows[i].update( dt );
		}
	}
}

//void Weapon::draw(const GLuint &programID) {
void Weapon::render( GLuint worldLocation, GLuint tintLocation ) {
	/*for (int i = 0; i < mMax; i++) {
		if (arrows[i].isAlive())
			arrows[i].render(programID);
	}*/

	for( int i=0; i<WEAPON_MAX_ARROWS; i++ )
	{
		if( mpArrows[i].isAlive() )
		{
			mpArrows[i].renderNonAni( worldLocation, tintLocation );
		}
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

/*Weapon::Weapon(bool owner, GameData* data)
{
	this->playerOwned = owner;
	mMax = 10;
	this->arrows = new Arrow[mMax];
	for (int i = 0; i < mMax; i++)
	{
		this->arrows[i].load( data, "Models/wallBox.mole" );
	}
	this->mStrength = 5;
}*/

void Weapon::loadSound(Sound * sound)
{
	for (int i = 0; i < WEAPON_MAX_ARROWS; i++)
		mpArrows[i].loadSound(sound);
}

Weapon::Weapon()
	: mStrength( 5 )
{
	
}

Weapon::~Weapon() 
{
	//delete[] arrows;
}