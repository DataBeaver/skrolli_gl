#include "instance.h"
#include "shader.h"

namespace SkrolliGL {

Instance::Instance(const Renderable &r):
	renderable(r)
{ }

void Instance::set_matrix(const Matrix &m)
{
	matrix = m;
}

void Instance::render(const RenderState &state) const
{
	RenderState inner_state = state;
	// Combine this instance's matrix with the incoming modelview matrix.
	inner_state.modelview_matrix = state.modelview_matrix*matrix;
	renderable.render(inner_state);
}

} // namespace SkrolliGL
