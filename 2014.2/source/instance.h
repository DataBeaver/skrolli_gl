#ifndef SKROLLIGL_OBJECTINSTANCE_H_
#define SKROLLIGL_OBJECTINSTANCE_H_

#include "mathutils.h"
#include "renderable.h"

namespace SkrolliGL {

class Object;

/*
An instance of a renderable.  Allows rendering multiple copies of the same
renderable without duplicating its data.  Each instance has a matrix which
determines the position and orientation of the renderable.

Instances can also be animated.  See the Animation and Engine classes for
details.
*/
class Instance: public Renderable
{
private:
	const Renderable &renderable;
	Matrix matrix;

public:
	Instance(const Renderable &);

	void set_matrix(const Matrix &);
	const Matrix &get_matrix() const { return matrix; }

	virtual void render(const RenderState &) const;
};

} // namespace SkrolliGL

#endif
