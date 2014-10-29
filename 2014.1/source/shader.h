#ifndef SKROLLIGL_SHADER_H_
#define SKROLLIGL_SHADER_H_

#include <map>
#include <string>
#include "mathutils.h"
#include "resourcemanager.h"

namespace SkrolliGL {

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

	void set_source(const std::string &, const std::string &);
private:
	static void set_shader_source(int, const std::string &);

public:
	virtual void load(const ResourceManager &, const std::string &);

	void bind();

	int get_uniform_location(const std::string &);

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
