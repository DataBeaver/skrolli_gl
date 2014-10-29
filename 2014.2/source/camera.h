#ifndef SKROLLIGL_CAMERA_H_
#define SKROLLIGL_CAMERA_H_

#include "mathutils.h"

namespace SkrolliGL {

/*
Determines the point of view from which the scene is rendered.
*/
class Camera
{
private:
	Vector position;
	float field_of_view;
	float aspect_ratio;
	float near_clip;
	float far_clip;
	float heading;
	float pitch;
	float roll;
	Matrix projection_matrix;
	Matrix view_matrix;

public:
	Camera();

	/* Sets vertical field of view, in degrees.  Horizontal field of view is
	computed with aspect ratio. */
	void set_field_of_view(float);

	/* Sets the ratio of width to height of the viewport.  If this does not
	match the physical viewport, the rendered image will be distorted. */
	void set_aspect_ratio(float);

	/* Sets the range of visible depth values.  Extremely large ratios between
	near and far clip planes will cause the dwepth buffer to lose accuracy. */
	void set_depth_range(float, float);

	/* Sets the position of the camera. */
	void set_position(const Vector &);

	/* Sets the heading of the camera, in degrees.  0° is the positive X axis,
	with positive angles turning counter-clockwise. */
	void set_heading(float);

	/* Sets the pitch of the camera, in degrees.  0° is horizontal, positive
	angles are up. */
	void set_pitch(float);

	/* Sets the roll of the camera, in degrees.  0° is upright, with positive
	angles causing the camera to roll counter-clockwise. */
	void set_roll(float);

	/* Sets heading and pitch so that the camera looks at the given point from
	its current position. */
	void look_at(const Vector &);

	const Vector &get_position() const { return position; }
	float get_heading() const { return heading; }
	float get_pitch() const { return pitch; }
	float get_roll() const { return roll; }

private:
	void recompute_view_matrix();
	void recompute_projection_matrix();
public:
	const Matrix &get_view_matrix() const { return view_matrix; }
	const Matrix &get_projection_matrix() const { return projection_matrix; }
};

} // namespace SkrolliGL

#endif
