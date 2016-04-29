#pragma once
#include <GL\glew.h>
#include <glm\mat4x4.hpp>
class TestSprite {
public:
	TestSprite();
	~TestSprite();

	void init(float x, float y, float width, float height);
	void draw(const GLuint &shaderProg);

private:
	int _x;
	int _y;
	int _width;
	int _height;

	glm::mat4x4 worldMat;

	GLuint mVertexArray;
	GLuint mIndexBuffer;
	GLuint _vboID;
};
