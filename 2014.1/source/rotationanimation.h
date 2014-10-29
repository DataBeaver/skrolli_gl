#ifndef SKROLLIGL_ROTATIONANIMATION_H_
#define SKROLLIGL_ROTATIONANIMATION_H_

#include "animation.h"

namespace SkrolliGL {

class RotationAnimation: public Animation
{
private:
	Matrix base_translation;
	Matrix base_rotation;
	char axis;
	float angle;

public:
	RotationAnimation(Instance &, char, float, float, EasingType = CUBIC);
	RotationAnimation(Instance &, char, float);
private:
	void init(char, float);

	virtual Matrix compute_matrix(float);
};

} // namespace SkrolliGL

#endif
