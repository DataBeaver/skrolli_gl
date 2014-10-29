#ifndef OBJECT_H_
#define OBJECT_H_

#include <vector>
#include "mathutils.h"

class Shader;
class Texture;

/* Handy constants for vertex attributes. */
enum VertexAttribute
{
	POSITION,
	NORMAL,
	TEXCOORD,
	TANGENT,
	BINORMAL
};

/* A class holding the data for a complete object */
class Object
{
public:
	/* A structure describing a three dimensional vertex with a normal. */
	struct Vertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float tx, ty, tz;
		float bx, by, bz;
	};

private:
	unsigned vertex_buffer_id;
	unsigned index_buffer_id;
	unsigned vertex_array_id;
	unsigned n_indices;

	Shader *shader;
	Texture *texture;
	Texture *normalmap;
	Matrix matrix;

	Object(const Object &);
	Object &operator=(const Object &);
public:
	Object(const std::vector<Vertex> &, const std::vector<unsigned> &);
	~Object();

private:
	template<typename T>
	static void set_attrib_array(unsigned index, unsigned size, float T::*member);

public:
	/// Sets the shader used to render the object.
	void set_shader(Shader *);

	/// Sets the texture used to render the object.  Normalmap is set to null.
	void set_texture(Texture *t) { set_texture(t, 0); }

	/// Sets the texture and normalmap used to render the object.
	void set_texture(Texture *, Texture *);

	/// Sets the model matrix of the object.
	void set_matrix(const Matrix &);

	/// Renders the object.  View matrix is given as an argument.
	void render(const Matrix &) const;
};

#endif
