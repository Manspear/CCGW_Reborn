#include "Model.h"

bool Model::load( Assets* assets, std::string file )
{
	bool result = true;

	MoleReader moleReader;
	moleReader.readFromBinary( file.c_str() );

	mMeshes = moleReader.getMeshList()->size();
	mTextures = moleReader.getMaterialList()->size();

	mpMeshes = new Mesh[mMeshes];
	mpTextures = new Texture*[mTextures];

	for( int i=0; i<mMeshes && result; i++ )
		result = result && mpMeshes[i].load( &moleReader, i );

	for( int i=0; i<mTextures && result; i++ )
	{
		mpTextures[i] = assets->load<Texture>( moleReader.getMaterial(i)->diffuseTexture );
		if( mpTextures[i] == nullptr )
			result = false;
	}

	return result;
}

void Model::unload()
{
	for( int i=0; i<mMeshes; i++ )
		mpMeshes[i].unload();

	delete[] mpMeshes;
	delete[] mpTextures;

	mMeshes = mTextures = 0;

	// No need to unload the textures, Assets will take care of that
}

void Model::draw()
{
	for( int i=0; i<mMeshes; i++ )
	{
		int tex = mpMeshes[i].getTextureIndex();
		if( tex < mTextures )
			mpTextures[tex]->bind();

		mpMeshes[i].draw();
	}
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