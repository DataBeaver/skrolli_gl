#ifndef SHADER_H_
#define SHADER_H_

#include <map>
#include <string>
#include "mathutils.h"

class Shader
{
private:
	unsigned vertex_shader_id;
	unsigned fragment_shader_id;
	unsigned program_id;
	std::map<std::string, int> uniforms;

	Shader(const Shader &);
	Shader &operator=(const Shader &);
public:
	Shader(const std::string &, const std::string &);
	~Shader();

private:
	static unsigned create_shader(int, const std::string &);

public:
	void use();

	int get_uniform_location(const std::string &);

	void set_uniform(const std::string &, int);
	void set_uniform(const std::string &, float, float, float);
	void set_uniform(const std::string &, float, float, float, float);
	void set_uniform_matrix4(const std::string &, const float *);
	void set_uniform(const std::string &n, const Vector &v) { set_uniform(n, v.x, v.y, v.z); }
	void set_uniform(const std::string &n, const Matrix &m) { set_uniform_matrix4(n, m.m); }
};

#endif
