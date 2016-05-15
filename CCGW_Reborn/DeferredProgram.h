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

	//void enableTextures(GLuint nextShader) const;
	void enableTextures(GLuint* textureLocations) const;
	void disableTextures() const;

	GLuint getViewPerspectiveLocation() const;
	GLuint getCameraPositionLocation() const;
	GLuint getFrameBuffer() const;
	GLuint getWorldLocation() const;
	GLuint getAnimationLocation() const;

private:
	GLuint mFBOid;
	union
	{
		GLuint mTextures[5];
		struct
		{
			GLuint mAmbientTex;
			GLuint mDiffuseTex;
			GLuint mSpecularTex;
			GLuint mNormalTex;
			GLuint mDepthTex;
		};
	};
	GLuint mViewPerspectiveLocation;
	GLuint mCameraPositionLocation;
	GLuint mWorldLocation;
	GLuint mAnimationLocation;
	GLuint mTexSamplerLocation, mSpecularSamplerLocation, mNormalSamplerLocation;
};
