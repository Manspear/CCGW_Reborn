#pragma once
#include <gl\glew.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void compileShaders(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	void linkShaders();
	void addAttribute(const std::string& attributeName);
	void setClear(bool yesNo);

	virtual void use() = 0;
	virtual void unUse() = 0;
	GLuint getProgramID() const;
protected:
	GLuint mProgramID;
	GLuint mVertexShader;
	GLuint mPixelShader;
	GLuint mGeometryShader;
	bool mClearScreen;

	int mTotalAttributes;
	void compileShader(const string& content, GLuint shaderID);
	void findAttribute(const string &content);
	string readShader(const string &filePath);
};