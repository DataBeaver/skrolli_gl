#ifndef SKROLLIGL_TEXTURE_H_
#define SKROLLIGL_TEXTURE_H_

#include <string>
#include "resourcemanager.h"

namespace SkrolliGL {

/*
A two-dimensional image that can be displayed on the surface of an object, or
used in other creative ways.

Textures can be loaded from a number of image file formats, including PNG and
JPEG.
*/
class Texture: public Resource
{
public:
	enum Format
	{
		RGB,
		RGBA,
		RGB_FLOAT,
		RGBA_FLOAT
	};

private:
	unsigned id;

	Texture(const Texture &);
	Texture &operator=(const Texture &);
public:
	Texture();
	~Texture();

	/* Returns the OpenGL ID of the texture.  Not intended for external use. */
	unsigned get_id() const { return id; }

	/* Creates a texture with unspecified contents.  Mainly useful for the
	Framebuffer class. */
	void create(unsigned, unsigned, Format);

	/* Loads an image from a file.  Usually called by ResourceManager. */
	void load(const ResourceManager &, const std::string &);

	/* Sets wrapping mode for the texture.  When enabled (the default), the
	texture will be tiled indefinitely.  When disabled, texture coordinates
	outside of the range [0, 1] will be clamped to the texture's edges. */
	void set_wrap(bool);

	/* Makes the texture the current one for the given texture unit. */
	void bind(unsigned = 0);

	/* Unbinds a texture from the given texture unit. */
	static void unbind(unsigned = 0);
};

} // namespace SkrolliGL

#endif
