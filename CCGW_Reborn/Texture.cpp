#include "Texture.h"

bool Texture::load( Assets* assets, string file )
{
	bool result = false;

	SDL_Surface* img = IMG_Load(file.c_str());
	if (img)
	{
		GLenum format = GL_RGBA;
		if( img->format->BytesPerPixel == 3 )
			format = GL_RGB;
		if (img->format->BytesPerPixel == 1)
			format = GL_RED;

		glGenTextures(1, &mID);
		glBindTexture(GL_TEXTURE_2D, mID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		mWidth = img->w;
		mHeight = img->h;

		SDL_FreeSurface(img);
		glBindTexture(GL_TEXTURE_2D, 0);
		result = true;
	}
	
	return result;
}

void Texture::unload()
{
	if (mID > 0)
		glDeleteTextures(1, &mID);
	mWidth = mHeight = 0;
}

void Texture::bind( int location ) const
{
	glActiveTexture( location );
	glBindTexture( GL_TEXTURE_2D, mID );
}

GLuint Texture::getID() const
{
	return mID;
}

Texture& Texture::operator=(const Texture& ref)
{
	mID = ref.mID;
	mWidth = ref.mWidth;
	mHeight = ref.mHeight;
	return *this;
}

Texture::Texture(const Texture& ref)
	: mID(ref.mID), mWidth(ref.mWidth), mHeight(ref.mHeight)
{
}

Texture::Texture()
	: mID(0), mWidth(0), mHeight(0)
{
}

Texture::~Texture()
{
}