#include <iostream>
#include <stdexcept>
#include <GL/glew.h>
#include "object.h"
#include "shader.h"

using namespace std;

Shader::Shader(const string &vertex_src, const string &fragment_src)
{
	// Create and compile shaders.
	vertex_shader_id = create_shader(GL_VERTEX_SHADER, vertex_src);
	fragment_shader_id = create_shader(GL_FRAGMENT_SHADER, fragment_src);

	// Create a shader program and attach the shaders to it.
	program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);

	/* Bind input and output locations.  Must be done before linking the
	program.  It is not an error to bind names that are not used. */
	glBindAttribLocation(program_id, POSITION, "in_position");
	glBindAttribLocation(program_id, NORMAL, "in_normal");
	glBindAttribLocation(program_id, TEXCOORD, "in_texcoord");
	glBindAttribLocation(program_id, TANGENT, "in_tangent");
	glBindAttribLocation(program_id, BINORMAL, "in_binormal");
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

Shader::~Shader()
{
	glDeleteProgram(program_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
}

unsigned Shader::create_shader(int type, const string &src)
{
	// Create and compile the shader object
	unsigned shader_id = glCreateShader(type);
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

	return shader_id;
}

void Shader::use()
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
