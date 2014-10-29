#ifndef SKROLLIGL_OBJECTINSTANCE_H_
#define SKROLLIGL_OBJECTINSTANCE_H_

#include "mathutils.h"
#include "renderable.h"

namespace SkrolliGL {

class Object;

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
