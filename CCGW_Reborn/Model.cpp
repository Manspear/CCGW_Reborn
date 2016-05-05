#include "Model.h"

bool Model::load( Assets* assets, std::string file )
{
	MoleReader moleReader;
	moleReader.readFromBinary( file.c_str() );

	mMeshes = moleReader.getMeshList()->size();
	mTextures = moleReader.getMaterialList()->size();

	mpMeshes = new Mesh[mMeshes];
	mpTextures = new Texture*[mTextures];

	for( int i=0; i<mMeshes; i++ )
		mpMeshes[i].load( &moleReader, i );

	for( int i=0; i<mTextures; i++ )
		mpTextures[i] = assets->load<Texture>( moleReader.getMaterial(i)->diffuseTexture );

	return true;
}

void Model::unload()
{
}

Model& Model::operator=( const Model& ref )
{
	return *this;
}

Model::Model( const Model& ref )
{
}

Model::Model()
{
}

Model::~Model()
{
}