#ifndef SKROLLIGL_TRANSLATIONANIMATION_H_
#define SKROLLIGL_TRANSLATIONANIMATION_H_

#include "animation.h"

namespace SkrolliGL {

class TranslationAnimation: public Animation
{
private:
	Matrix base_matrix;
	Vector movement;

public:
	TranslationAnimation(Instance &, const Vector &, float, EasingType = CUBIC);

private:
	virtual Matrix compute_matrix(float);
};

} // namespace SkrolliGL

#endif
