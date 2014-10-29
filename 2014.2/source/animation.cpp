#include "animation.h"
#include "instance.h"

namespace SkrolliGL {

Animation::Animation(Instance &i, float d, EasingType e):
	instance(i),
	duration(d),
	elapsed(0),
	easing(e)
{ }

float Animation::get_progress() const
{
	float p = elapsed;
	if(duration>0)
	{
		p /= duration;
		if(easing==CUBIC)
			p = (3*p-2)*p*p;
	}
	return p;
}

void Animation::advance(float dt)
{
	elapsed += dt;
	if(duration>0 && elapsed>=duration)
		elapsed = duration;

	instance.set_matrix(compute_matrix(get_progress()));
}

} // namespace SkrolliGL
