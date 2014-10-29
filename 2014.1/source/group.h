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
