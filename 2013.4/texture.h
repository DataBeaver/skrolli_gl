#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>

class Texture
{
private:
	unsigned id;

	Texture(const Texture &);
	Texture &operator=(const Texture &);
public:
	Texture(const std::string &);
	~Texture();

	void bind();

	static void unbind();
};

#endif
