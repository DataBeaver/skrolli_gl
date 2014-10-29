#include <stdexcept>
#include <GL/glew.h>
#include "object.h"
#include "shader.h"
#include "texture.h"

using namespace std;

Object::Object(const vector<Vertex> &vertices, const vector<unsigned> &indices):
	n_indices(indices.size()),
	shader(0),
	texture(0),
	normalmap(0)
{
	// Create vertex array object first.
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Create a buffer and transfer the vertex data into it.
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Ditto for index data.
	glGenBuffers(1, &index_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

	// Set up attribute arrays
	set_attrib_array(POSITION, 3, &Vertex::x);
	set_attrib_array(NORMAL, 3, &Vertex::nx);
	set_attrib_array(TEXCOORD, 2, &Vertex::u);
	set_attrib_array(TANGENT, 3, &Vertex::tx);
	set_attrib_array(BINORMAL, 3, &Vertex::bx);
}

Object::~Object()
{
	glDeleteVertexArrays(1, &vertex_array_id);
	glDeleteBuffers(1, &vertex_buffer_id);
	glDeleteBuffers(1, &index_buffer_id);
}

template<typename T>
void Object::set_attrib_array(unsigned index, unsigned size, float T::*member)
{
	T dummy;
	const char *offset_base = reinterpret_cast<const char *>(&dummy);
	unsigned offset = reinterpret_cast<const char *>(&(dummy.*member))-offset_base;
	glVertexAttribPointer(index, size, GL_FLOAT, false, sizeof(T), reinterpret_cast<void *>(offset));
	glEnableVertexAttribArray(index);
}

void Object::set_shader(Shader *s)
{
	shader = s;
}

void Object::set_texture(Texture *t, Texture *n)
{
	texture = t;
	normalmap = n;
}

void Object::set_matrix(const Matrix &m)
{
	matrix = m;
}

void Object::render(const Matrix &vm) const
{
	// Can't render anything without a shader
	if(!shader)
		return;

	// Bind shader and textures
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	if(texture)
		texture->bind();
	else
		Texture::unbind();
	glActiveTexture(GL_TEXTURE1);
	if(normalmap)
		normalmap->bind();
	else
		Texture::unbind();

	// Combine model and view matrices to create modelview matrix.
	Matrix mvm = vm*matrix;
	shader->set_uniform("modelview", mvm);
	Vector light = vm.transform_direction(Vector(0, -0.5, 0.833));
	shader->set_uniform("light_direction", light);

	glBindVertexArray(vertex_array_id);
	glDrawElements(GL_TRIANGLE_STRIP, n_indices, GL_UNSIGNED_INT, 0);
}
