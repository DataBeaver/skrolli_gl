#include <stdexcept>
#include "instance.h"
#include "rotationanimation.h"

using namespace std;

namespace SkrolliGL {

RotationAnimation::RotationAnimation(Instance &i, char x, float a, float d, EasingType e):
	Animation(i, d, e)
{
	init(x, a);
}

RotationAnimation::RotationAnimation(Instance &i, char x, float a):
	Animation(i, -1, LINEAR)
{
	init(x, a);
}

void RotationAnimation::init(char x, float a)
{
	base_rotation = instance.get_matrix();
	for(unsigned i=12; i<15; ++i)
		swap(base_translation.m[i], base_rotation.m[i]);

	axis = x;
	angle = a;
	if(axis!='X' && axis!='Y' && axis!='Z')
		throw invalid_argument("RotationAnimation::RotationAnimation");
}

Matrix RotationAnimation::compute_matrix(float p)
{
	Matrix matrix;
	if(axis=='X')
		matrix = Matrix::rotation_x(angle*p);
	else if(axis=='Y')
		matrix = Matrix::rotation_y(angle*p);
	else if(axis=='Z')
		matrix = Matrix::rotation_z(angle*p);
	return base_translation*matrix*base_rotation;
}

} // namespace SkrolliGL
