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

	Texture* getDiffuseMap( int index ) const;
	Texture* getSpecularMap( int index ) const;
	Texture* getNormalMap( int index ) const;

	Model& operator=( const Model& ref );
	Model( const Model& ref );
	Model();
	~Model();

private:
	Mesh* mpMeshes;
	int mMeshes;

	Texture **mpDiffuseMaps, **mpSpecularMaps, **mpNormalMaps;
	int mMaps;
};