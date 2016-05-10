#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "GameObject.h"
#include <gl\glew.h>

class Camera
{
public:
	/*Makes the camera follow a target offset by the supplied distance.*/
	void follow( glm::vec3 position, const glm::vec3 &lookAt, float distance , const glm::vec3 &up);
	//void update(const GLuint &programID);
	void tacticalMovement(glm::vec3 moveDir, const float &height);
	/*Sets the view, projection and camera position of the shader.*/
	void updateUniforms( GLuint viewPerspective, GLuint cameraPosition );

	void setPerspective( float fov, float aspectRatio, float nearplane = 0.01f, float farplane = 100.0f );
	void setPosition( glm::vec3 position );
	glm::vec3 getPosition() const;
	const glm::mat4& getView() const;
	const glm::mat4& getPerspective() const;
	glm::mat4 getViewPerspective() const;

	Camera& operator=( const Camera& ref );
	Camera( const Camera& ref );
	Camera( float fov, float aspectRatio, float nearplane = 0.01f, float farplane = 100.0f );
	~Camera();

private:
	glm::vec3 mPosition;
	glm::mat4 mView;
	glm::mat4 mPerspective;
};