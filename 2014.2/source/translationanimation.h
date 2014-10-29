#ifndef SKROLLIGL_TRANSLATIONANIMATION_H_
#define SKROLLIGL_TRANSLATIONANIMATION_H_

#include "animation.h"

namespace SkrolliGL {

/*
Animates an Instance with a linear movement.
*/
class TranslationAnimation: public Animation
{
private:
	Matrix base_matrix;
	Vector movement;

public:
	/* Constructs a fixed-duration TranslationAnimation with a target position. */
	TranslationAnimation(Instance &, const Vector &to, float duration, EasingType = CUBIC);

	/* Constructs a continuous TranslationAnimation with a speed. */
	TranslationAnimation(Instance &, const Vector &speed);

private:
	virtual Matrix compute_matrix(float);
};

} // namespace SkrolliGL

#endif
