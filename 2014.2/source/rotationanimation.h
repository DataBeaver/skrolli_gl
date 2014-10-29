#ifndef SKROLLIGL_ROTATIONANIMATION_H_
#define SKROLLIGL_ROTATIONANIMATION_H_

#include "animation.h"

namespace SkrolliGL {

/*
Animates an Instance by rotating it around an axis.
*/
class RotationAnimation: public Animation
{
private:
	Matrix base_translation;
	Matrix base_rotation;
	char axis;
	float angle;

public:
	/* Constructs a fixed-duration RotationAnimation with a total rotation
	angle.  Axis must be one of 'X', 'Y', 'Z'. */
	RotationAnimation(Instance &, char axis, float angle, float duration, EasingType = CUBIC);

	/* Constructs a continuous RotationAnimation with a rotation rate.  Axis
	must be one of 'X', 'Y', 'Z'. */
	RotationAnimation(Instance &, char axis, float rate);

private:
	void init(char, float);

	virtual Matrix compute_matrix(float);
};

} // namespace SkrolliGL

#endif
