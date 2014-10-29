#ifndef SKROLLIGL_ANIMATION_H_
#define SKROLLIGL_ANIMATION_H_

#include "mathutils.h"

namespace SkrolliGL {

class Instance;

/*
Base class for animations.  Individual animation classes must override the
compute_delta_matrix method to specify the kind of movement.
*/
class Animation
{
public:
	enum EasingType
	{
		LINEAR,
		CUBIC
	};

protected:
	Instance &instance;
	float duration;
	float elapsed;
	EasingType easing;

	Animation(Instance &, float, EasingType);
public:
	virtual ~Animation() { }

	Instance &get_instance() const { return instance; }
	float get_progress() const;

	/* Advances the animation by the given amount of seconds. */
	void advance(float);

protected:
	/* Computes the matrix at a given point of the animation.  If the
	animation has a duration, p ranges from 0 to 1 during the animation.  For
	continuous animations, p is the elapsed time in seconds. */
	virtual Matrix compute_matrix(float p) = 0;

public:
	bool has_finished() const { return duration>0 && elapsed>=duration; }
};

} // namespace SkrolliGL

#endif
