#include <cmath>
#include "mathutils.h"

using namespace std;

namespace SkrolliGL {

Matrix::Matrix()
{
	for(unsigned i=0; i<16; ++i)
		m[i] = (i%5==0 ? 1.0f : 0.0f);
}

Matrix Matrix::translation(float x, float y, float z)
{
	Matrix matrix;
	matrix.m[12] = x;
	matrix.m[13] = y;
	matrix.m[14] = z;
	return matrix;
}

Matrix Matrix::rotation_x(float a)
{
	a *=M_PI/180;
	Matrix matrix;
	matrix.m[5] = cos(a);
	matrix.m[6] = sin(a);
	matrix.m[9] = -sin(a);
	matrix.m[10] = cos(a);
	return matrix;
}

Matrix Matrix::rotation_y(float a)
{
	a *=M_PI/180;
	Matrix matrix;
	matrix.m[0] = cos(a);
	matrix.m[2] = -sin(a);
	matrix.m[8] = sin(a);
	matrix.m[10] = cos(a);
	return matrix;
}

Matrix Matrix::rotation_z(float a)
{
	a *=M_PI/180;
	Matrix matrix;
	matrix.m[0] = cos(a);
	matrix.m[1] = sin(a);
	matrix.m[4] = -sin(a);
	matrix.m[5] = cos(a);
	return matrix;
}

Matrix Matrix::frustum(float fov, float aspect, float near_clip, float far_clip)
{
	float height = tan(fov/2)*2*near_clip;
	float width = height*aspect;
	float depth = far_clip-near_clip;

	Matrix result;
	result.m[0] = 2*near_clip/width;
	result.m[5] = 2*near_clip/height;
	result.m[10] = -(far_clip+near_clip)/depth;
	result.m[11] = -1;
	result.m[14] = -2*far_clip*near_clip/depth;
	result.m[15] = 0;
	return result;
}

Matrix Matrix::operator*(const Matrix &rhs) const
{
	Matrix result;
	for(unsigned i=0; i<4; ++i)
		for(unsigned j=0; j<4; ++j)
		{
			float value = 0;
			for(unsigned k=0; k<4; ++k)
				value += m[k*4+j]*rhs.m[i*4+k];
			result.m[i*4+j] = value;
		}
	return result;
}

Vector Matrix::transform(const Vector &v) const
{
	return Vector(v.x*m[0]+v.y*m[4]+v.z*m[8]+m[12],
		v.x*m[1]+v.y*m[5]+v.z*m[9]+m[13],
		v.x*m[2]+v.y*m[6]+v.z*m[10]+m[14]);
}

Vector Matrix::transform_direction(const Vector &v) const
{
	return Vector(v.x*m[0]+v.y*m[4]+v.z*m[8],
		v.x*m[1]+v.y*m[5]+v.z*m[9],
		v.x*m[2]+v.y*m[6]+v.z*m[10]);
}

} // namespace SkrolliGL
