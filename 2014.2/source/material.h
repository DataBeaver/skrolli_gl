#ifndef SKROLLIGL_MATERIAL_H_
#define SKROLLIGL_MATERIAL_H_

#include "resourcemanager.h"

namespace SkrolliGL {

class Shader;
class Texture;

/*
Describes the appearance of a surface.

The file format for materials is line-based.  Each line has a keyword followed
by values.  Lines starting with a hash ('#') are ignored.  The following
keywords are recognized:

shader <name>

  Sets the shader for the material.

texture <name>

  Sets the texture for the material.

uniform <name> <values>

  Sets a uniform value.  Between one and four floating-point values can be
  specified.

The canonical filename extension is .mat.
*/
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

	/* Sets the shader for the material.  A null shader may result in
	unpredictable behavior. */
	void set_shader(Shader *);

	/* Sets the texture for the material.  The texture will be bound to textuer
	unit zero. */
	void set_texture(Texture *);

	Shader *get_shader() const { return shader; }
	Texture *get_texture() const { return texture; }

	/* Loads the material from a file.  Usually called by ResourceManager.  Any
	shader or texture referenced by the file must be already known by the
	ResourceManager. */
	virtual void load(const ResourceManager &, const std::string &);

	/* Makes the material active. */
	void apply() const;
};

} // namespace SkrolliGL

#endif
