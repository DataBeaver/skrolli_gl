#ifndef SKROLLIGL_MATHUTILS_H_
#define SKROLLIGL_MATHUTILS_H_

#include <cmath>

namespace SkrolliGL {

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

	Vector transform(const Vector &) const;
	Vector transform_direction(const Vector &) const;
};

} // namespace SkrolliGL

#endif
