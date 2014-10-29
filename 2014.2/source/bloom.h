#ifndef SKROLLIGL_BLOOM_H_
#define SKROLLIGL_BLOOM_H_

#include "framebuffer.h"
#include "material.h"
#include "object.h"
#include "postprocessor.h"
#include "shader.h"

namespace SkrolliGL {

/*
Renders a bloom effect, spreading out very bright spots in the image.
*/
class Bloom: public Postprocessor
{
private:
	unsigned width;
	unsigned height;
	float scaling;
	Framebuffer scene_fbo;
	Framebuffer hblur_fbo;
	Framebuffer vblur_fbo;
	Shader blur_shader;
	Shader combine_shader;
	Material material;
	Object object;

public:
	Bloom(unsigned, unsigned);

	/* Sets the standard deviation radius of the blur.  A radius much larger
	than 5.0 may cause visible clipping artifacts to occur. */
	void set_radius(float);

	/* Sets radius scaling value.  This increases the blur radius without risk
	of clipping, but reduces the effect around small details. */
	void set_scaling(float);

	/* Sets */
	void set_strenght(float);

	virtual Framebuffer &get_render_target() { return scene_fbo; }
	virtual void render_effect(Framebuffer *);
};

} // namespace SkrolliGL

#endif
