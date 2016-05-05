#pragma once

#include "Mesh.h"
#include "Texture.h"
#include <Mole\MoleReader.h>

class Model : public Asset
{
public:
	bool load( Assets* assets, std::string file );
	void unload();
	void draw();

	Model& operator=( const Model& ref );
	Model( const Model& ref );
	Model();
	~Model();

private:
	Mesh* mpMeshes;
	int mMeshes;

	Texture** mpTextures;
	int mTextures;
};