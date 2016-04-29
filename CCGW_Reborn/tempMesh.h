#pragma once

#include "Assets.h"
#include <GL\glew.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <GLM\glm.hpp>

class tempMesh : public Asset
{
public:
	/*Load a mesh from a file. Returns true if the load was successful.*/
	bool load(string file);
	/*Unloads the mesh and frees the memory on the GPU.*/
	void unload();
	void draw();

	tempMesh& operator=(const tempMesh& ref);
	tempMesh(const tempMesh& ref);
	tempMesh();
	~tempMesh();

private:
	GLuint mVertexArray;
	GLuint mVertexBuffer;
	GLuint mIndexBuffer;
	int mSize;
};