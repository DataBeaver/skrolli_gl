#ifndef SKROLLIGL_RESOURCEMANAGER_H_
#define SKROLLIGL_RESOURCEMANAGER_H_

#include <list>
#include <map>
#include <string>

namespace SkrolliGL {

class Resource;

class ResourceManager
{
private:
	typedef std::map<std::string, Resource *> ResourceMap;

	ResourceMap resources;

public:
	ResourceManager();
	~ResourceManager();

	void load_directory(const std::string &);
private:
	void load_files(const std::string &, const std::list<std::string> &, const std::string &, void (ResourceManager::*)(const std::string &, const std::string &));
	template<typename T>
	void load_resource(const std::string &, const std::string &);

public:
	Resource &get(const std::string &) const;

	template<typename T>
	T &get(const std::string &name) const
	{
		// dynamic_casting a reference will throw a bad_cast if the type is wrong
		return dynamic_cast<T &>(get(name));
	}
};

class Resource
{
protected:
	Resource() { }

public:
	virtual ~Resource() { }

	virtual void load(const ResourceManager &, const std::string &) = 0;
};

} // namespace SkrolliGL

#endif
