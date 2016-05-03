#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <vector>

ShaderProgram::ShaderProgram(): mProgramID(0), mVertexShader(0), mPixelShader(0), mTotalAttributes(0), mGeometryShader(0), mClearScreen(true) {

}

ShaderProgram::~ShaderProgram() {

}

void ShaderProgram::compileShaders(const std::string& vertexPath, const std::string& pixelPath, const std::string& geometryPath) {
	mProgramID = glCreateProgram();
	mVertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (mVertexShader == 0) 
		std::cout << "Vertex shader problem";
	string vertexContent = readShader(vertexPath);
	compileShader(vertexContent, mVertexShader);
	findAttribute(vertexContent);

	mPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (mPixelShader == 0) 
		std::cout << "Fragment shader problem!";
	
	if (geometryPath != " ")
	{
		mGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		if (mGeometryShader == 0) {
			std::cout << "Geometry shader problem!";
		}
		compileShader(readShader(geometryPath), mGeometryShader);
	}
	compileShader(readShader(pixelPath), mPixelShader);
	linkShaders();
}

void ShaderProgram::compileShader(const std::string& content, GLuint shaderID) {
	const char* contentsPtr = content.c_str();
	glShaderSource(shaderID, 1, &contentsPtr, nullptr);
	glCompileShader(shaderID);
	GLint success = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

		glDeleteShader(shaderID);
		std::printf("%s\n", &(errorLog[0]));
	}
}

string ShaderProgram::readShader(const std::string& filePath) {
	std::ifstream shaderFile("shaders/" + filePath);
	if (shaderFile.fail()) {
		std::cout << "Fil fel: " + filePath;
	}

	std::stringstream fileContent;
	std::string line;
	while (std::getline(shaderFile, line)) {
		fileContent << line << "\n";
	}

	shaderFile.close();
	return fileContent.str();
}

void ShaderProgram::findAttribute(const string &content) {
	istringstream s;
	s.str(content);
	string temp,temp1,temp2,temp3;
	istringstream s2;
	while (getline(s, temp)) {
		if (temp.substr(0,3) == "in ") {
			s2.str(temp);
			s2 >> temp1 >> temp2 >> temp3;
			addAttribute(temp3);
		}
	}
}

void ShaderProgram::linkShaders() {
	glAttachShader(mProgramID, mVertexShader);
	if (mGeometryShader != 0)
		glAttachShader(mProgramID, mGeometryShader);
	glAttachShader(mProgramID, mPixelShader);
	glLinkProgram(mProgramID);
	GLint success = 0;
	glGetProgramiv(mProgramID, GL_LINK_STATUS, (int*)&success);
	if (success == GL_FALSE) {
		cout << "Linknings fel i shaders " << endl;
	}
	glDetachShader(mProgramID, mVertexShader);
	if (mGeometryShader != 0) {
		glDetachShader(mProgramID, mGeometryShader);
		glDeleteShader(mGeometryShader);
	}
	glDetachShader(mProgramID, mPixelShader);
	glDeleteShader(mVertexShader);
	glDeleteShader(mPixelShader);
}

void ShaderProgram::addAttribute(const string &name) {
	glBindAttribLocation(mProgramID, mTotalAttributes++, name.c_str());
}

void ShaderProgram::setClear(bool yesNo)
{
	mClearScreen = yesNo;
}

GLuint ShaderProgram::getProgramID() const {
	return mProgramID;
}