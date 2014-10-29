#ifndef SKROLLIGL_SHADER_H_
#define SKROLLIGL_SHADER_H_

#include <map>
#include <string>
#include "mathutils.h"
#include "resourcemanager.h"

namespace SkrolliGL {

/*
Shaders are used to compute the position and color of drawn primitives.  They
are written in GLSL.

The file format for shaders consists of vertex shader source, followed by a
line consisting of dash characters ('-'), followed by fragment shader source.
The canonical filename extension is .glsl.
*/
class Shader: public Resource
{
private:
	unsigned vertex_shader_id;
	unsigned fragment_shader_id;
	unsigned program_id;
	std::map<std::string, int> uniforms;

	Shader(const Shader &);
	Shader &operator=(const Shader &);
public:
	Shader();
	~Shader();

	/* Sets the source code for the shader from strings. */
	void set_source(const std::string &vertex_src, const std::string &fragment_src);
private:
	static void set_shader_source(int, const std::string &);

public:
	/* Loads shader source code from a file.  Usually called by ResourceManager. */
	virtual void load(const ResourceManager &, const std::string &);

	/* Binds the shader to be used for rendering. */
	void bind();

	/* Returns the location of a uniform variable.  A location of -1 means that
	the uniform does not exist.  Use the set_uniform functions to set the values
	of uniform variables. */
	int get_uniform_location(const std::string &);

	/* Sets the value of a uniform variable. */
	void set_uniform(const std::string &, int);
	void set_uniform(const std::string &, float);
	void set_uniform(const std::string &, float, float);
	void set_uniform(const std::string &, float, float, float);
	void set_uniform(const std::string &, float, float, float, float);
	void set_uniform_matrix4(const std::string &, const float *);
	void set_uniform(const std::string &n, const Vector &v) { set_uniform(n, v.x, v.y, v.z); }
	void set_uniform(const std::string &n, const Matrix &m) { set_uniform_matrix4(n, m.m); }
};

} // namespace SkrolliGL

#endif
