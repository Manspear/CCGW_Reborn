#include "Shadow.h"

Shadow::Shadow(const std::string& vertexPath, const std::string& fragmentPath) : ShaderProgram()
{
	compileShaders(vertexPath, fragmentPath, " ");

	glUseProgram(mProgramID);
	mWorldLocation = glGetUniformLocation(mProgramID, "world");
	mAnimationLocation = glGetUniformLocation(mProgramID, "animationMatrices");
	glUseProgram(0);


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 512, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);


	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexture, 0);

	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "No SHADOW framebuffer!!!!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->position = {10,30,30};
	//kannske andra hållet
	vpmat =glm::lookAt(position, { 0,0,0 }, { 0,01,0 })*glm::perspective(45.0f, 512.0f/2048.0f,1.0f,50.0f);
	invvpmat = glm::inverse(vpmat);
}
Shadow::~Shadow()
{

}
void Shadow::use()
{
	glUseProgram(mProgramID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, 512, 2048);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}
void Shadow::use(GLuint frambuffer)
{

}
void Shadow::unUse()
{
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Shadow::updateUniforms()
{
	GLuint location = glGetUniformLocation(mProgramID, "VPmat");
	glUniformMatrix4fv(location, 1, GL_FALSE, &vpmat[0][0]);
}
glm::vec3 Shadow::getPositionOfLight() {
	return position;
}
GLuint Shadow::getWorldLocation()
{
	return mWorldLocation;
}
GLuint Shadow::getAnimationLocation() 
{
	return mAnimationLocation;
}
GLuint Shadow::getDepthText() 
{
	return this->shadowTexture;
}
glm::mat4 Shadow::getMat()
{
	return vpmat;
}

glm::mat4 Shadow::getInvMat() 
{
	return invvpmat;
}