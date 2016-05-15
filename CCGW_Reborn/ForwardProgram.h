#pragma once
#include "ShaderProgram.h"
class ForwardProgram : public ShaderProgram {
public:
	void use();
	void unUse();

	ForwardProgram();
	ForwardProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	~ForwardProgram();

	GLuint* getTextureLocations() const;
	GLuint getCameraPositionLocation() const;
	GLuint getInverseViewPerspectiveLocation() const;

private:
	union
	{
		GLuint mTextureLocations[5];
		struct
		{
			GLuint mAmbientLocation;
			GLuint mDiffuseLocation;
			GLuint mSpecularLocation;
			GLuint mNormalLocation;
			GLuint mDepthLocation;
		};
	};
	GLuint mCameraPositionLocation;
	GLuint mInverseViewPerspectiveLocation;
};
