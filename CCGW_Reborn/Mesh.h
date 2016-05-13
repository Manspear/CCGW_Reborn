#pragma once

#include "Assets.h"
#include <GL\glew.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <GLM\glm.hpp>

#include "Mole\MoleReader.h"

class Mesh : public Asset
{
public:
	/*Load a mesh from a file. Returns true if the load was successful.*/
	bool load( Assets* assets, string file );
	bool load( MoleReader* reader, int index );
	/*Unloads the mesh and frees the memory on the GPU.*/
	void unload();
	void draw();
	void drawNonAni();

	int getMaterialIndex() const;
	
	Mesh& operator=(const Mesh& ref);
	Mesh(const Mesh& ref);
	Mesh();
	~Mesh();

private:
	GLuint mVertexArray;
	GLuint mVertexBuffer;
	GLuint mIndexBuffer;
	int mSize;
	int mMaterialIndex;
};