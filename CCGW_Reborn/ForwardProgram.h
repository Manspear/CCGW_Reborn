#pragma once
#include "ShaderProgram.h"
class ForwardProgram : public ShaderProgram {
public:
	void use();
	void unUse();

	ForwardProgram();
	ForwardProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
	~ForwardProgram();
private:
	
};
