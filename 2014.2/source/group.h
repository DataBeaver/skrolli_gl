#ifndef SKROLLIGL_GROUP_H_
#define SKROLLIGL_GROUP_H_

#include <vector>
#include "renderable.h"
#include "resourcemanager.h"

namespace SkrolliGL {

class Instance;

/*
A group of Renderables.  Groups are useful for managing scenes or compound
Renderables.  Since a Group is itself a Renderable, it can be added to another
Group, an Instance may be created of it, etc.

The file format for groups is line-based.  Each line has a keyword followed by
values.  Lines starting with a hash ('#') are ignored.  The following keywords
are recognized:

object <name>

  Adds a new object to the group.

translate <x> <y> <z>

  Translates the latest object by the vector (x y z).

rotate_x <angle>
rotate_y <angle>
rotate_z <angle>

  Rotates the latest object by an angle around a primary axis.

Any translation and rotation operations occur withing the already-transformed
coordinate system.  So a rotation around Z axis by 45 degrees followed by a
translation by (1 0 0) will translate the object in a direction halfway between
the global X and Y axes.

The canonical filename extension is .scene.
*/
class Group: public Renderable, public Resource
{
private:
	std::vector<const Renderable *> contents;
	std::vector<Instance *> instances;

public:
	~Group();

	/* Adds a Renderable to the Group. */
	void add(const Renderable &);

	/* Removes a Renderable from the Group. */
	void remove(const Renderable &);

	/* Loads contents for the Group from a file.  Existing contents are not
	removed.  The Group will take care of deleting any newly-created objects. */
	virtual void load(const ResourceManager &, const std::string &);

	virtual void render(const RenderState &) const;
};

} // namespace SkrolliGL

#endif
