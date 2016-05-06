#include "GameObject.h"
#include <iostream>

/*bool GameObject::load(Mesh * assets, Texture * diffuseTex, Texture * specularMap, Texture * normalMap)
{
	if (assets != nullptr)
		mpMesh2 = assets;
	if (diffuseTex != nullptr)
		mpTexture = diffuseTex;
	if (specularMap != nullptr)
		mpSpecularMap = specularMap;
	if (normalMap != nullptr)
		mpNormalMap = normalMap;

	return (mpMesh != nullptr);
}

bool GameObject::load(tempMesh* assets, Texture* diffuseTex, Texture* specularMap, Texture* normalMap)
{
	if (assets != nullptr)
		mpMesh = assets;
	if (diffuseTex != nullptr)
		mpTexture = diffuseTex;
	if (specularMap != nullptr)
		mpSpecularMap = specularMap;
	if (normalMap != nullptr)
		mpNormalMap= normalMap;

	return (mpMesh != nullptr);
}

bool GameObject::loadTex(Texture* texture) {
	mpTexture = texture;
	return (mpTexture != nullptr);
}*/

bool GameObject::load( Model* model )
{
	mpModel = model;
	return true;
}

void GameObject::update(const float &dt) {
}

void GameObject::render(const GLuint & programID)
{
	GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->mWorld[0][0]);

	//activateTextures(programID);
	//mpMesh->draw();
	mpModel->draw();
	//deactivateTextures();
}

void GameObject::render(const GLuint & programID, const glm::mat4 &viewMat)
{
	GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->mWorld[0][0]);

	//activateTextures(programID);
	//mpMesh->draw();
	mpModel->draw();
	//deactivateTextures();
}

/*void GameObject::render2(const GLuint & programID)
{
	GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->mWorld[0][0]);
	activateTextures(programID);
	//mpMesh2->draw();
	mpModel->draw();
	deactivateTextures();
}*/

/*void GameObject::activateTextures(const GLuint &programID) {
	GLuint texLocation = glGetUniformLocation(programID, "texSampler");
	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mpTexture->getID());
	if (mpSpecularMap != nullptr) {
		GLuint specularLocation = glGetUniformLocation(programID, "specularSampler");
		glUniform1i(specularLocation, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mpSpecularMap->getID());
	}
	if (mpNormalMap != nullptr) {
		GLuint normalLocation = glGetUniformLocation(programID, "normalSampler");
		glUniform1i(normalLocation, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mpNormalMap->getID());
	}
}

void GameObject::deactivateTextures() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
}*/

glm::vec3 GameObject::getPosition() const
{
	return mPosition;
}

void GameObject::setPosition( glm::vec3 position )
{
	mPosition = position;
	mWorld[3][0] = position.x;
	mWorld[3][1] = position.y;
	mWorld[3][2] = position.z;
}

void GameObject::setScale(float scale)
{
	//mWorld = { scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1 };
	mWorld[0][0] = scale;
	mWorld[1][1] = scale;
	mWorld[2][2] = scale;
}

GameObject::GameObject(const GameObject& ref)
	: mWorld(ref.mWorld), rotX(0),rotY(0), mpModel( ref.mpModel )
{

}

GameObject::GameObject(glm::vec3 position = { 0, 0, 0 }, float scale = 1.0f)
{
	mPosition = position;
	this->mBB = BoundingBox(mPosition, scale);
	mWorld = {
				scale,			0,				0,				0,
				0,				scale,			0,				0,
				0,				0,				scale,			0,
				position.x,		position.y,		position.z,		1.0 
	};

}

GameObject::GameObject()
	:scale(1.0f), rotX(0), rotY(0), mpModel( nullptr )
{
	mPosition = { 0, 0, 0 };
	mLookat = { 0, 0, -1 };
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	this->mBB = BoundingBox(mPosition, scale);
}

GameObject::~GameObject()
{

}