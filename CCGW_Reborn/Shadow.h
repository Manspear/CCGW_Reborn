#pragma once
#include <glm\glm.hpp>
#include <gl\glew.h>
#include "ShaderProgram.h"
#include "glm\gtx\transform.hpp"
//#include <glm\gtx\>

class Shadow : ShaderProgram
{
	public:
		Shadow(const std::string& vertexPath, const std::string& fragmentPath);
		~Shadow();

		void use();
		void use(GLuint frambuffer);
		void unUse();
		GLuint getDepthText();
		glm::vec3 getPositionOfLight();
		void updateUniforms();
		GLuint getWorldLocation();
		GLuint getAnimationLocation();
		glm::mat4 getInvMat();
		glm::mat4 getMat();
	private:
		glm::mat4 vpmat;
		glm::mat4 invvpmat;
		GLuint frameBuffer;
		GLuint shadowTexture;
		glm::vec3 position;

		GLuint mWorldLocation;
		GLuint mAnimationLocation;
};