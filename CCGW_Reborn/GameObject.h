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
	/*Abstract method. Don't call.*/
	bool load( Model* model );
	//bool loadTex(Texture* texture);
	/*Abstract method. Don't call.*/
	virtual void update(const float &dt);
	/*Abstract method. Don't call.*/
	virtual void render(const GLuint &programID);
	//virtual void render2(const GLuint &programID);
	virtual void render(const GLuint &programID, const glm::mat4 &viewMat);
	glm::vec3 getPosition() const;
	void setPosition( glm::vec3 position );
	void setScale(float scale);
	void setWorld(glm::mat4 world);
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
	BoundingBox mBB;

	Model* mpModel;
};