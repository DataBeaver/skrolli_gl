#include <stdexcept>
#include <dirent.h>
#include "group.h"
#include "material.h"
#include "object.h"
#include "resourcemanager.h"
#include "shader.h"
#include "texture.h"

using namespace std;

namespace SkrolliGL {

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for(ResourceMap::iterator i=resources.begin(); i!=resources.end(); ++i)
		delete i->second;
}

void ResourceManager::load_directory(const string &path)
{
	DIR *dir = opendir(path.c_str());
	if(!dir)
		throw runtime_error("could not open "+path);

	list<string> files;
	while(dirent *de = readdir(dir))
		files.push_back(de->d_name);

	load_files(path, files, ".glsl", &ResourceManager::load_resource<Shader>);
	load_files(path, files, ".png", &ResourceManager::load_resource<Texture>);
	load_files(path, files, ".jpg", &ResourceManager::load_resource<Texture>);
	load_files(path, files, ".mat", &ResourceManager::load_resource<Material>);
	load_files(path, files, ".obj", &ResourceManager::load_resource<Object>);
	load_files(path, files, ".scene", &ResourceManager::load_resource<Group>);
}

void ResourceManager::load_files(const string &dir, const list<string> &files, const string &ext, void (ResourceManager::*callback)(const string &, const string &))
{
	for(list<string>::const_iterator i=files.begin(); i!=files.end(); ++i)
		if(i->size()>=ext.size() && !i->compare(i->size()-ext.size(), ext.size(), ext))
		{
			string full = dir+"/"+*i;
			(this->*callback)(*i, full);
		}
}

template<typename T>
void ResourceManager::load_resource(const string &name, const string &filename)
{
	if(resources.count(filename))
		return;

	T *resource = new T;
	try
	{
		resource->load(*this, filename);
	}
	catch(...)
	{
		delete resource;
		throw;
	}
	resources[name] = resource;
}

Resource &ResourceManager::get(const string &name) const
{
	ResourceMap::const_iterator i = resources.find(name);
	if(i==resources.end())
		throw runtime_error(name+" not found");
	return *i->second;
}

} // namespace SkrolliGL
