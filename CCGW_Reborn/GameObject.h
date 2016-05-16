#pragma once

#include <GLM\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "tempMesh.h"
#include "Texture.h"
#include "Input.h"
#include "GameData.h"
#include "global_variables.h"
#include "BoundingBox.h"
#include "Model.h"
#include "Sound.h"

class GameObject
{
public:
	bool load( Model* model );
	virtual void update(const float &dt);
	virtual void renderAni( GLuint worldLocation, GLuint animationLocation, int animationTake);
	virtual void renderNonAni( GLuint worldLocation );
	glm::vec3 getPosition() const;
	void setPosition( glm::vec3 position );
	void setScale(float scale);
	void setWorld(glm::mat4 world);
	void setAnimation( int animation );
	void loadSound(Sound* sound);

	GameObject(const GameObject& ref);
	GameObject(glm::vec3 position, float scale);
	GameObject();
	virtual ~GameObject();
protected: // <-- changed to protected from private. because i wanted to reach these from subclasses.
	glm::mat4 mWorld;
	glm::vec3 mPosition;
	glm::vec3 mLookat;
	float rotX;
	float rotY;
	float scale;
	Sound* mSound;
	float animationTime;
	int mAnimation;

	BoundingBox mBB;

	Model* mpModel;
};