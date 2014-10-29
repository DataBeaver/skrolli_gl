#include <fstream>
#include <iostream>
#include <stdexcept>
#include <GL/glew.h>
#include "object.h"
#include "shader.h"

using namespace std;

namespace SkrolliGL {

Shader::Shader():
	vertex_shader_id(glCreateShader(GL_VERTEX_SHADER)),
	fragment_shader_id(glCreateShader(GL_FRAGMENT_SHADER)),
	program_id(glCreateProgram())
{
	// Attach shaders to the program.
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
}

Shader::~Shader()
{
	glDeleteProgram(program_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

void Shader::set_source(const string &vertex_src, const string &fragment_src)
{
	// Create and compile shaders.
	set_shader_source(vertex_shader_id, vertex_src);
	set_shader_source(fragment_shader_id, fragment_src);

	/* Bind input and output locations.  Must be done before linking the
	program.  It is not an error to bind names that are not used. */
	glBindAttribLocation(program_id, Object::POSITION, "in_position");
	glBindAttribLocation(program_id, Object::NORMAL, "in_normal");
	glBindAttribLocation(program_id, Object::TEXCOORD, "in_texcoord");
	glBindFragDataLocation(program_id, 0, "out_color");

	// Link the shader program.
	glLinkProgram(program_id);

	// Check for linking errors.
	int status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &status);
	if(!status)
	{
		char buf[1024];
		glGetProgramInfoLog(program_id, sizeof(buf), NULL, buf);
		cerr<<"Shader link error:"<<endl<<buf<<endl;
		throw runtime_error("Failed to link shader");
	}    
}

void Shader::set_shader_source(int shader_id, const string &src)
{
	// Create and compile the shader object
	const char *src_ptr = src.c_str();
	glShaderSource(shader_id, 1, &src_ptr, NULL);
	glCompileShader(shader_id);

	// Check for compilation errors.
	int status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if(!status)
	{
		char buf[1024];
		glGetShaderInfoLog(shader_id, sizeof(buf), NULL, buf);
		cerr<<"Shader compilation error:"<<endl<<buf<<endl;
		throw runtime_error("Failed to compile shader");
	}
}

void Shader::load(const ResourceManager &, const string &filename)
{
	ifstream input(filename.c_str());
	string line;
	unsigned part = 0;
	string vertex_src;
	string fragment_src;

	while(getline(input, line))
	{
		// Check if the line consists entirely of dashes
		bool separator = true;
		for(string::const_iterator i=line.begin(); (separator && i!=line.end()); ++i)
			separator = (*i=='-');

		if(separator)
		{
			++part;
			if(part>1)
				throw runtime_error("Shader file has more than two parts");
			continue;
		}

		if(part==0)
		{
			vertex_src += line;
			vertex_src += '\n';
		}
		else if(part==1)
		{
			fragment_src += line;
			fragment_src += '\n';
		}
	}

	if(part==0)
		throw runtime_error("Shader file has only one part");

	set_source(vertex_src, fragment_src);
}

void Shader::bind()
{
	glUseProgram(program_id);
}

int Shader::get_uniform_location(const string &name)
{
	// Check from the cache first
	map<string, int>::iterator i = uniforms.find(name);
	if(i!=uniforms.end())
		return i->second;

	// Look up the location and put it in the cache
	int loc = glGetUniformLocation(program_id, name.c_str());
	uniforms[name] = loc;

	return loc;
}

void Shader::set_uniform(const string &name, int i)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniform1i(loc, i);
}

void Shader::set_uniform(const string &name, float x)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniform1f(loc, x);
}

void Shader::set_uniform(const string &name, float x, float y)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniform2f(loc, x, y);
}

void Shader::set_uniform(const string &name, float x, float y, float z)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniform3f(loc, x, y, z);
}

void Shader::set_uniform(const string &name, float x, float y, float z, float w)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniform4f(loc, x, y, z, w);
}

void Shader::set_uniform_matrix4(const string &name, const float *m)
{
	int loc = get_uniform_location(name);
	if(loc>=0)
		glUniformMatrix4fv(loc, 1, false, m);
}

} // namespace SkrolliGL
