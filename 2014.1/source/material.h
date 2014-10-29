#ifndef SKROLLIGL_MATERIAL_H_
#define SKROLLIGL_MATERIAL_H_

#include "resourcemanager.h"

namespace SkrolliGL {

class Shader;
class Texture;

class Material: public Resource
{
private:
	struct Uniform
	{
		std::string name;
		unsigned n_elems;
		float values[4];
	};

	Shader *shader;
	Texture *texture;
	std::list<Uniform> uniforms;

public:
	Material();

	void set_shader(Shader *);
	Shader *get_shader() const { return shader; }
	void set_texture(Texture *);
	Texture *get_texture() const { return texture; }

	virtual void load(const ResourceManager &, const std::string &);

	void apply() const;
};

} // namespace SkrolliGL

#endif
