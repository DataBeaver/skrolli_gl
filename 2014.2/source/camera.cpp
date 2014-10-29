#include <cmath>
#include "camera.h"

using namespace std;

namespace SkrolliGL {

Camera::Camera():
	field_of_view(60),
	aspect_ratio(4.0/3.0),
	near_clip(0.1),
	far_clip(100),
	heading(0),
	pitch(0),
	roll(0)
{
	recompute_projection_matrix();
	recompute_view_matrix();
}

void Camera::set_field_of_view(float fov)
{
	field_of_view = fov;
	recompute_projection_matrix();
}

void Camera::set_aspect_ratio(float ar)
{
	aspect_ratio = ar;
	recompute_projection_matrix();
}

void Camera::set_depth_range(float n, float f)
{
	near_clip = n;
	far_clip = f;
	recompute_projection_matrix();
}

void Camera::set_position(const Vector &p)
{
	position = p;
	recompute_view_matrix();
}

void Camera::set_heading(float h)
{
	heading = h;
	recompute_view_matrix();
}

void Camera::set_pitch(float p)
{
	pitch = p;
	recompute_view_matrix();
}

void Camera::set_roll(float r)
{
	roll = r;
	recompute_view_matrix();
}

void Camera::look_at(const Vector &p)
{
	float dx = p.x-position.x;
	float dy = p.y-position.y;
	heading = atan2(dy, dx)*180.0f/M_PI;
	float dxy = sqrt(dx*dx+dy*dy);
	float dz = p.z-position.z;
	pitch = atan2(dz, dxy)*180.0f/M_PI;
	recompute_view_matrix();
}

void Camera::recompute_view_matrix()
{
	// First rotate to the default orientation (looking towards X+, with Z+ up)
	view_matrix = Matrix::rotation_x(-90)
		* Matrix::rotation_z(90)
	// Rotate from camera coordinates to world coordinates
		* Matrix::rotation_x(roll)
		* Matrix::rotation_y(pitch)
		* Matrix::rotation_z(-heading)
	// Finally, translate to world origin
		* Matrix::translation(-position);
}

void Camera::recompute_projection_matrix()
{
	projection_matrix = Matrix();
	float near_height = tan(field_of_view/2*M_PI/180.0f)*near_clip*2;
	float near_width = near_height*aspect_ratio;
	float depth_range = far_clip-near_clip;
	projection_matrix.m[0] = 2*near_clip/near_width;
	projection_matrix.m[5] = 2*near_clip/near_height;
	projection_matrix.m[10] = -(far_clip+near_clip)/depth_range;
	projection_matrix.m[11] = -1;
	projection_matrix.m[14] = -2*far_clip*near_clip/depth_range;
	projection_matrix.m[15] = 0;
}

} // namespace SkrolliGL
