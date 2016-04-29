#include "TestSprite.h"
#include <cstddef>

struct Vertex {
	float x, y, z;
	float u, v;
	GLbyte r, g, b;
};

TestSprite::TestSprite() {
	_vboID = 0;
}


TestSprite::~TestSprite() {
	if (_vboID != 0)
		glDeleteBuffers(1, &_vboID);
}

void TestSprite::init(float x, float y, float width, float height) {
	_x = x;
	_y = y;
	_width = width;
	_height = height;

	if (_vboID == 0) {
		glGenBuffers(1, &_vboID);
	}

	Vertex vertexData[6];

	//first triangle
	vertexData[0].x = x + width;
	vertexData[0].y = y + height;
	vertexData[0].z = -1;

	vertexData[1].x = x;
	vertexData[1].y = y + height;
	vertexData[1].z = -1;

	vertexData[2].x = x;
	vertexData[2].y = y;
	vertexData[2].z = -1;

	//second triangle
	vertexData[3].x = x;
	vertexData[3].y = y;
	vertexData[3].z = -1;

	vertexData[4].x = x + width;
	vertexData[4].y = y;
	vertexData[4].z = -1;

	vertexData[5].x = x + width;
	vertexData[5].y = y + height;
	vertexData[5].z = -1;

	for (int i = 0; i < 6; i++)
	{
		vertexData[i].r = 200;
		vertexData[i].g = 120;
		vertexData[i].b = 80;
		vertexData[i].u = 0;
		vertexData[i].v = 1;
	}

	/*glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(Vertex, x));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(Vertex, u));
	glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)offsetof(Vertex, r));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	worldMat = glm::mat4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void TestSprite::draw(const GLuint &shaderProg) {
	GLuint world = glGetUniformLocation(shaderProg, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->worldMat[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
	glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}