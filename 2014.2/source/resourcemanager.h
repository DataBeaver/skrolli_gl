#ifndef SKROLLIGL_RESOURCEMANAGER_H_
#define SKROLLIGL_RESOURCEMANAGER_H_

#include <list>
#include <map>
#include <string>

namespace SkrolliGL {

class Resource;

/*
Loads resources from files and provides access to them by name.  The following
resource types and filename extensions are recognized:

Shader: .glsl
Texture: .png, .jpg
Material: .mat
Object: .obj
Group: .scene

See the descriptions of the individual classes for descriptions of the file
formats.
*/
class ResourceManager
{
private:
	typedef std::map<std::string, Resource *> ResourceMap;

	ResourceMap resources;

public:
	ResourceManager();
	~ResourceManager();

	/* Loads all recognized resource files from a directory. */
	void load_directory(const std::string &);
private:
	void load_files(const std::string &, const std::list<std::string> &, const std::string &, void (ResourceManager::*)(const std::string &, const std::string &));
	template<typename T>
	void load_resource(const std::string &, const std::string &);

public:
	/* Gets a loaded resource.  See also the template version. */
	Resource &get(const std::string &) const;

	/* Gets a loaded resource of a particular type. */
	template<typename T>
	T &get(const std::string &name) const
	{
		// dynamic_casting a reference will throw a bad_cast if the type is wrong
		return dynamic_cast<T &>(get(name));
	}
};

/*
Base class for all resources.
*/
class Resource
{
protected:
	Resource() { }

public:
	virtual ~Resource() { }

	/* Loads the resource from a file.  The filename is suitable for opening the
	file directly. */
	virtual void load(const ResourceManager &, const std::string &filename) = 0;
};

} // namespace SkrolliGL

#endif
