#include <stdexcept>
#include <SDL_image.h>
#include <GL/glew.h>
#include "texture.h"

using namespace std;

namespace SkrolliGL {

Texture::Texture()
{
	// Create the OpenGL texture object
	glGenTextures(1, &id);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::create(unsigned w, unsigned h, Format f)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	int ifmt;
	if(f==RGB)
		ifmt = GL_RGB;
	else if(f==RGBA)
		ifmt = GL_RGBA;
	else if(f==RGB_FLOAT)
		ifmt = GL_RGB16F;
	else if(f==RGBA_FLOAT)
		ifmt = GL_RGBA16F;
	else
		throw invalid_argument("Invalid texture format");
	glTexImage2D(GL_TEXTURE_2D, 0, ifmt, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void Texture::load(const ResourceManager &, const string &filename)
{
	SDL_Surface *image = IMG_Load(filename.c_str());
	if(!image)
		throw runtime_error("Could not load "+filename);

	int fmt;
	if(image->format->format==SDL_PIXELFORMAT_RGB24)
		fmt = GL_RGB;
	else if(image->format->format==SDL_PIXELFORMAT_ABGR8888)
		fmt = GL_RGBA;
	else
	{
		SDL_FreeSurface(image);
		throw runtime_error("Don't know how to handle format of "+filename);
	}

	glBindTexture(GL_TEXTURE_2D, id);

	// Set minification filter to something that doesn't need mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Upload the image data
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, image->w, image->h, 0, fmt, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);
}

void Texture::set_wrap(bool w)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (w ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (w ? GL_REPEAT : GL_CLAMP_TO_EDGE));
}

void Texture::bind(unsigned unit)
{
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind(unsigned unit)
{
	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace SkrolliGL
