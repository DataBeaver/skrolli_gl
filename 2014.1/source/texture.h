#ifndef SKROLLIGL_TEXTURE_H_
#define SKROLLIGL_TEXTURE_H_

#include <string>
#include "resourcemanager.h"

namespace SkrolliGL {

class Texture: public Resource
{
private:
	unsigned id;

	Texture(const Texture &);
	Texture &operator=(const Texture &);
public:
	Texture();
	~Texture();

	void load(const ResourceManager &, const std::string &);

	void bind();

	static void unbind();
};

} // namespace SkrolliGL

#endif
