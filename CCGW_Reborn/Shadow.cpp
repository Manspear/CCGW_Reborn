#include "Shadow.h"
#include "global_variables.h"

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
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 2048, 0, GL_RED, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);


	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, DrawBuffers);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &frameBuffer); 
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexture, 0);

	glDrawBuffer(GL_NONE);


	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "No SHADOW framebuffer!!!!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->position = {15,100,50};
	//kannske andra hållet
	vpmat =glm::perspective(55.0f, 2048.0f/2048.0f,1.0f, 150.0f) * glm::lookAt(position, {15,0,50 }, { 0,0,1 });
	//invvpmat = glm::inverse(vpmat);
}
Shadow::~Shadow()
{

}
void Shadow::use()
{
	glUseProgram(mProgramID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, 2048, 2048);
	glClearDepth(1.0);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
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

void Shadow::enableTexture(GLuint texLoc) {
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glUniform1i(texLoc, 6);
}