#ifndef SKROLLIGL_MATHUTILS_H_
#define SKROLLIGL_MATHUTILS_H_

#include <cmath>

namespace SkrolliGL {

/*
A three-dimensional vector.
*/
class Vector
{
public:
	float x, y, z;

	Vector(): x(0), y(0), z(0) { }
	Vector(float x_, float y_, float z_): x(x_), y(y_), z(z_) { }

	Vector operator+(const Vector &v) const { return Vector(x+v.x, y+v.y, z+v.z); }
	Vector operator-(const Vector &v) const { return Vector(x-v.x, y-v.y, z-v.z); }
	Vector operator-() const { return Vector(-x, -y, -z); }
	Vector operator*(float s) const { return Vector(x*s, y*s, z*s); }
	float dot(const Vector &v) const { return x*v.x+y*v.y+z*v.z; }
	Vector cross(const Vector &v) const { return Vector(y*v.z-v.y*z, z*v.x-v.z*x, x*v.y-v.x*y); }
	float length() const { return std::sqrt(dot(*this)); }
};

/*
A 4×4 matrix, suitable for representing affine transformations in three-
dimensional space.
*/
class Matrix
{
public:
	float m[16];

	Matrix();

	static Matrix translation(float, float, float);
	static Matrix translation(const Vector &v) { return translation(v.x, v.y, v.z); }
	static Matrix rotation_x(float);
	static Matrix rotation_y(float);
	static Matrix rotation_z(float);
	static Matrix frustum(float, float, float, float);

	Matrix operator*(const Matrix &) const;

	/* Transforms a vector by the matrix. */
	Vector transform(const Vector &) const;

	/* Transforms a vector by the linear part of the matrix.  This ignores any
	translation that might be present and is suitable for transforming
	directional vectors. */
	Vector transform_direction(const Vector &) const;
};

} // namespace SkrolliGL

#endif
