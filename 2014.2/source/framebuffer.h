#ifndef SKROLLIGL_FRAMEBUFFER_H_
#define SKROLLIGL_FRAMEBUFFER_H_

#include "texture.h"

namespace SkrolliGL {

/*
Provides a render-to-texture target.
*/
class Framebuffer
{
private:
	unsigned width;
	unsigned height;
	unsigned id;
	Texture color_tex;
	unsigned depth_buf_id;

	static int system_viewport[4];

public:
	/* Constructs a framebuffer with the specified dimensions. */
	Framebuffer(unsigned w, unsigned h);

	~Framebuffer();

	/* Selects a floating-point format for the color buffer. */
	void set_float(bool);

	/* Returns the color buffer associated with the framebuffer. */
	Texture &get_color_buffer() { return color_tex; }

	/* Adds a depth buffer.  Rendering a scene usually requires a depth buffer,
	but intermediate framebuffers of postprocessing effects don't. */
	void add_depth_buffer();

	void bind();

	static void unbind();
};

} // namespace SkrolliGL

#endif
