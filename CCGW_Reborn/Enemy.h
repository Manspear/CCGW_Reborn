#pragma once

#include "GameObject.h"
#include "Grid.h"
#include "BoundingBox.h"
#include "glm\gtx\vector_angle.hpp"
#include "Emitter.h"
#include "Texture.h"

#define ENEMY_RENDER_HITBOX 0

//Abstract class for enemies.
class Enemy : public GameObject
{
public:
	//Render the enemy.
	//void render( GLuint programID );
	void render( GLuint worldLocation, GLuint animationLocation );

	//Set the path that this enemy should follow.
	void setPath( sNode* path, int max );
	void load(Model* model, Emitter* pEmitter);
	void imHit(float strength, glm::vec3 position);
	void setAlive( bool alive );
	void setLife( float life );
	bool getAlive() const;
	float getLife() const;
	const BoundingBox& getBoundingBox() const;
	const BoundingBox& getHeadBox() const;

	Enemy& operator=( const Enemy& ref );
	Enemy( const Enemy& ref );
	Enemy( glm::vec3 position );
	Enemy();
	~Enemy();

	static Model* pBoundingBoxModel;
protected:
	
	Emitter mEmitter;
	BoundingBox mBoundingBox;
	BoundingBox mHeadBox;
	float mBoundRadius;
	sNode* pPath;
	int mCurrent;
	float mLife;
	bool mAlive;
};