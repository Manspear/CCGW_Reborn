#include "Model.h"

#define SLOW_LAUNCH 1

bool Model::load( Assets* assets, std::string file )
{
	bool result = true;

	MoleReader moleReader;
#if SLOW_LAUNCH
	moleReader.readFromBinary( file.c_str() );
#else
	moleReader.readFromBinary( "Models/wallBox.mole" );
#endif

	mMeshes = moleReader.getMeshList()->size();
	mMaps = moleReader.getMaterialList()->size();

	mpMeshes = new Mesh[mMeshes];
	mpDiffuseMaps = new Texture*[mMaps];
	mpSpecularMaps = new Texture*[mMaps];
	mpNormalMaps = new Texture*[mMaps];

	for( int i=0; i<mMeshes && result; i++ )
		result = result && mpMeshes[i].load( &moleReader, i );

	for( int i=0; i<mMaps && result; i++ )
	{
		const sMaterial* material = moleReader.getMaterial(i);
		mpDiffuseMaps[i] = assets->load<Texture>( material->diffuseTexture );
		mpSpecularMaps[i] = assets->load<Texture>( material->specularTexture );
		mpNormalMaps[i] = assets->load<Texture>( material->normalTexture );

		if (mpDiffuseMaps[i] == nullptr)
		{
			mpDiffuseMaps[i] = assets->load<Texture>("Models/cube.png");
		}
	}

	return result;
}

void Model::unload()
{
	for( int i=0; i<mMeshes; i++ )
		mpMeshes[i].unload();

	delete[] mpMeshes;
	delete[] mpDiffuseMaps;
	delete[] mpSpecularMaps;
	delete[] mpNormalMaps;

	mMeshes = mMaps = 0;

	// No need to unload the textures, Assets will take care of that
}

void Model::draw()
{
	for( int i=0; i<mMeshes; i++ )
	{
		int material = mpMeshes[i].getMaterialIndex();

		mpDiffuseMaps[material]->bind( GL_TEXTURE0 );

		Texture* spec = mpSpecularMaps[material];
		if( spec )
			spec->bind( GL_TEXTURE1 );
		else
		{
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		Texture* norm = mpNormalMaps[material];
		if( norm )
			norm->bind( GL_TEXTURE2 );
		else
		{
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, 0 ); 
		}

		mpMeshes[i].draw();
	}
}

Texture* Model::getDiffuseMap( int index ) const 
{
	return mpDiffuseMaps[index];
}

Texture* Model::getSpecularMap( int index ) const
{
	return mpSpecularMaps[index];
}

Texture* Model::getNormalMap( int index ) const
{
	return mpNormalMaps[index];
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