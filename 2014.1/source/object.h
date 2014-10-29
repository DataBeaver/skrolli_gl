#ifndef SKROLLIGL_OBJECT_H_
#define SKROLLIGL_OBJECT_H_

#include <string>
#include <vector>
#include "mathutils.h"
#include "renderable.h"
#include "resourcemanager.h"

namespace SkrolliGL {

class Material;

/*
Stores a single mesh with a material.
*/
class Object: public Resource, public Renderable
{
public:
	/* Handy constants for vertex attributes. */
	enum VertexAttribute
	{
		POSITION,
		NORMAL,
		TEXCOORD
	};

	/* A structure describing a three dimensional vertex with a normal and a
	texture coordinate. */
	struct Vertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

private:
	unsigned vertex_buffer_id;
	unsigned index_buffer_id;
	unsigned vertex_array_id;
	unsigned n_indices;

	Material *material;

public:
	Object();
	~Object();

private:
	void set_attrib_array(unsigned, unsigned, float Vertex::*);

public:
	/* Sets vertex and index data for the object.  The indices must form a
	single triangle strip. */
	void set_data(const std::vector<Vertex> &, const std::vector<unsigned> &);

	/* Sets the Material for the Object.  A null Material is permitted, but an
	Object can't be rendered without one. */
	void set_material(Material *);

	Material *get_material() const { return material; }

	/* Loads data for the Object from a file.  Currently only WaveFront .obj
	files are supported. */
	virtual void load(const ResourceManager &, const std::string &);
private:
	void load_obj(const ResourceManager &, const std::string &);

public:
	virtual void render(const RenderState &) const;
};

} // namespace SkrolliGL

#endif
