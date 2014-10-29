#ifndef SKROLLIGL_POSTPROCESSOR_H_
#define SKROLLIGL_POSTPROCESSOR_H_

#include "material.h"
#include "object.h"
#include "shader.h"

namespace SkrolliGL {

class Framebuffer;

/*
Interface for post processing effects.
*/
class Postprocessor
{
protected:
	Postprocessor() { }
public:
	virtual ~Postprocessor() { }

	/* Returns the framebuffer that should be used as the target for the
	previous rendering stage. */
	virtual Framebuffer &get_render_target() = 0;

	/* Renders the effect into the target framebuffer.  A null target means that
	the system framebuffer should be used. */
	virtual void render_effect(Framebuffer *target) = 0;
};

} // namespace SkrolliGL

#endif
