#include <stdexcept>
#include <SDL_image.h>
#include <GL/glew.h>
#include "texture.h"

using namespace std;

Texture::Texture(const string &filename)
{
	SDL_Surface *image = IMG_Load(filename.c_str());
	if(!image)
		throw runtime_error("Could not load "+filename);

	// Create the OpenGL texture object
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// Set minification filter to something that doesn't need mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Upload the image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
