#include "instance.h"
#include "translationanimation.h"

namespace SkrolliGL {

TranslationAnimation::TranslationAnimation(Instance &i, const Vector &to, float d, EasingType e):
	Animation(i, d, e),
	base_matrix(instance.get_matrix()),
	movement(to-base_matrix.transform(Vector()))
{ }

Matrix TranslationAnimation::compute_matrix(float p)
{
	return Matrix::translation(movement*p)*base_matrix;
}

} // namespace SkrolliGL
