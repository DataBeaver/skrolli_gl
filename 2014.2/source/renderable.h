#ifndef SKROLLIGL_RENDERABLE_H_
#define SKROLLIGL_RENDERABLE_H_

#include "mathutils.h"

namespace SkrolliGL {

/*
Holds global render state.  A RenderState instance is passed to each Renderable
during the rendering of a frame.
*/
struct RenderState
{
	Matrix projection_matrix;
	Matrix modelview_matrix;
	Vector light_direction;
	Vector sky_direction;
	float light_intensity;
	float ambient_intensity;
};

/*
An abstract base class for things that can be rendered.
*/
class Renderable
{
protected:
	Renderable() { }

public:
	virtual ~Renderable() { }

	/* Renders the renderable.  It is permissible to modify the shader program
	binding, the texture binding for texture unit 0 and the vertex array
	binding.  All other state must be restored after use. */
	virtual void render(const RenderState &) const = 0;
};

} // namespace SkrolliGL

#endif
