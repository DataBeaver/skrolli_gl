#include <fstream>
#include <sstream>
#include "material.h"
#include "shader.h"
#include "texture.h"

using namespace std;

namespace SkrolliGL {

Material::Material():
	shader(0),
	texture(0)
{ }

void Material::set_shader(Shader *s)
{
	shader = s;
}

void Material::set_texture(Texture *t)
{
	texture = t;
}

void Material::load(const ResourceManager &manager, const string &filename)
{
	ifstream input(filename.c_str());
	string line;

	while(getline(input, line))
	{
		if(line[0]=='#')
			continue;

		istringstream parse(line);
		string command;
		parse >> command;

		if(command=="shader")
		{
			string name;
			parse >> name;
			set_shader(&manager.get<Shader>(name+".glsl"));
		}
		else if(command=="texture")
		{
			string name;
			parse >> name;
			set_texture(&manager.get<Texture>(name));
		}
		else if(command=="uniform")
		{
			Uniform uni;
			parse >> uni.name;
			uni.n_elems = 0;
			for(unsigned i=0; i<4; ++i)
			{
				if(!(parse >> uni.values[i]))
					break;
				++uni.n_elems;
			}
			uniforms.push_back(uni);
		}
	}
}

void Material::apply() const
{
	if(!shader)
		return;

	shader->bind();

	for(list<Uniform>::const_iterator i=uniforms.begin(); i!=uniforms.end(); ++i)
	{
		if(i->n_elems==1)
			shader->set_uniform(i->name, i->values[0]);
		else if(i->n_elems==2)
			shader->set_uniform(i->name, i->values[0], i->values[1]);
		else if(i->n_elems==3)
			shader->set_uniform(i->name, i->values[0], i->values[1], i->values[2]);
		else if(i->n_elems==4)
			shader->set_uniform(i->name, i->values[0], i->values[1], i->values[2], i->values[3]);
	}

	if(texture)
	{
		texture->bind();
		shader->set_uniform("texture", 0);
	}
	else
		Texture::unbind();
}

} // namespace SkrolliGL
