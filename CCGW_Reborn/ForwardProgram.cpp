#include "ForwardProgram.h"
#include "global_variables.h"

ForwardProgram::ForwardProgram(){

}

ForwardProgram::ForwardProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) : ShaderProgram() {
	compileShaders(vertexPath, fragmentPath, geometryPath);
	
	glUseProgram(mProgramID);

	mAmbientLocation = glGetUniformLocation(mProgramID, "ambientT");
	mDiffuseLocation = glGetUniformLocation(mProgramID, "diffuseT");
	mSpecularLocation = glGetUniformLocation(mProgramID, "specularT");
	mNormalLocation = glGetUniformLocation(mProgramID, "normalT");
	mDepthLocation = glGetUniformLocation(mProgramID, "depthT");
	mShadowLocation = glGetUniformLocation(mProgramID, "shadowT");

	mCameraPositionLocation = glGetUniformLocation(mProgramID, "cameraPos");
	mInverseViewPerspectiveLocation = glGetUniformLocation(mProgramID, "invViewPerspective");

	glUseProgram(0);
}

ForwardProgram::~ForwardProgram() {

}

void ForwardProgram::use() {
	glUseProgram(mProgramID);
	for (int i = 0; i < mTotalAttributes; i++)
	{
		glEnableVertexAttribArray(i);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gWidth, gHeight);
	if (mClearScreen) {
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void ForwardProgram::unUse() {
	glUseProgram(0);
	for (int i = 0; i < mTotalAttributes; i++)
	{
		glDisableVertexAttribArray(i);
	}
}

GLuint* ForwardProgram::getTextureLocations() const {
	return (GLuint*)mTextureLocations;
}

GLuint ForwardProgram::getCameraPositionLocation() const {
	return mCameraPositionLocation;
}

GLuint ForwardProgram::getInverseViewPerspectiveLocation() const {
	return mInverseViewPerspectiveLocation;
}