#pragma once
#include "ShaderProgram.h"

class DeferredProgram : public ShaderProgram {
public:
	DeferredProgram();
	DeferredProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	~DeferredProgram();

	void use();
	void use(GLuint frambuffer);
	void unUse(); 

	void enableTextures(GLuint nextShader) const;
	void disableTextures() const;

	GLuint getViewPerspectiveLocation() const;
	GLuint getCameraPositionLocation() const;
	GLuint getFrameBuffer() const;

private:
	GLuint mFBOid;
	GLuint mNormalTex;
	GLuint mDiffuseTex;
	GLuint mAmbientTex;
	GLuint mSpecularTex;
	GLuint mDepthTex;
	GLuint mViewPerspectiveLocation;
	GLuint mCameraPositionLocation;
};
