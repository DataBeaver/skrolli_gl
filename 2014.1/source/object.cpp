#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <GL/glew.h>
#include "material.h"
#include "object.h"
#include "shader.h"
#include "texture.h"

using namespace std;

namespace SkrolliGL {

struct VertexRef
{
	int vertex;
	int normal;
	int texcoord;

	VertexRef();

	bool operator<(const VertexRef &) const;
};

struct Face
{
	unsigned char nverts;
	bool used;
	unsigned indices[4];

	Face();
};

istream &operator>>(istream &, VertexRef &);


Object::Object():
	n_indices(0),
	material(0)
{
	// Create vertex array object first.
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Create a buffer and transfer the vertex data into it.
	glGenBuffers(1, &vertex_buffer_id);

	// Ditto for index data.
	glGenBuffers(1, &index_buffer_id);
}

Object::~Object()
{
	glDeleteVertexArrays(1, &vertex_array_id);
	glDeleteBuffers(1, &vertex_buffer_id);
	glDeleteBuffers(1, &index_buffer_id);
}

void Object::set_attrib_array(unsigned index, unsigned size, float Vertex::*member)
{
	// Use a dummy Vertex to compute member offsets
	static Vertex dummy;
	const char *offset_base = reinterpret_cast<const char *>(&dummy);
	unsigned offset = reinterpret_cast<const char *>(&(dummy.*member))-offset_base;
	glVertexAttribPointer(index, size, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void *>(offset));
	glEnableVertexAttribArray(index);
}

void Object::set_data(const vector<Vertex> &vertices, const vector<unsigned> &indices)
{
	// Transfer vertex data to vertex buffer ...
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// ... and index data to index buffer.
	n_indices = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices*sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

	// Set up attribute arrays.  This can't be done before the buffer has some
	// data in it.
	set_attrib_array(POSITION, 3, &Vertex::x);
	set_attrib_array(NORMAL, 3, &Vertex::nx);
	set_attrib_array(TEXCOORD, 2, &Vertex::u);
}

void Object::set_material(Material *m)
{
	material = m;
}

void Object::load(const ResourceManager &manager, const string &filename)
{
	string::size_type dot = filename.rfind('.');
	string ext = filename.substr(dot);
	if(ext==".obj")
		load_obj(manager, filename);
	else
		throw runtime_error("Don't know how to load "+filename);
}

void Object::load_obj(const ResourceManager &manager, const string &filename)
{
	ifstream input(filename.c_str());
	string line;
	vector<Vector> positions;
	vector<Vector> texcoords;
	vector<Vector> normals;
	map<VertexRef, unsigned> vertex_map;

	vector<Vertex> vertices;
	vector<Face> faces;

	while(getline(input, line))
	{
		// Lines starting with # are comments
		if(line[0]=='#')
			continue;

		istringstream parse(line);
		string command;
		parse >> command;

		if(command=="o")
			throw runtime_error("Multiple objects per file not supported");

		if(command=="v")
		{
			Vector v;
			parse >> v.x >> v.y >> v.z;
			positions.push_back(v);
		}
		else if(command=="vt")
		{
			Vector vt;
			parse >> vt.x >> vt.y;
			texcoords.push_back(vt);
		}
		else if(command=="vn")
		{
			Vector vn;
			parse >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
		else if(command=="f")
		{
			Face face;
			for(unsigned i=0; i<4; ++i)
			{
				VertexRef vref;
				if(!(parse >> vref))
					break;

				map<VertexRef, unsigned>::iterator j = vertex_map.find(vref);
				unsigned index;
				if(j!=vertex_map.end())
					index = j->second;
				else
				{
					Vertex vertex;
					if(vref.vertex>=0)
					{
						vertex.x = positions[vref.vertex].x;
						vertex.y = positions[vref.vertex].y;
						vertex.z = positions[vref.vertex].z;
					}
					if(vref.normal>=0)
					{
						vertex.nx = normals[vref.normal].x;
						vertex.ny = normals[vref.normal].y;
						vertex.nz = normals[vref.normal].z;
					}
					if(vref.texcoord>=0)
					{
						vertex.u = texcoords[vref.texcoord].x;
						vertex.v = texcoords[vref.texcoord].y;
					}
					vertices.push_back(vertex);
					index = vertices.size()-1;
					vertex_map[vref] = index;
				}

				face.indices[i] = index;
				++face.nverts;
			}
			faces.push_back(face);
		}
		else if(command=="usemtl")
		{
			string name;
			parse >> name;
			set_material(&manager.get<Material>(name+".mat"));
		}
	}

	// Construct index array for triangle strips
	vector<unsigned> indices;
	indices.reserve(faces.size()*4);
	while(1)
	{
		// Search for an unused face
		vector<Face>::iterator face;
		for(face=faces.begin(); face!=faces.end(); ++face)
			if(!face->used)
				break;

		// Have all faces been processed?
		if(face==faces.end())
			break;

		// Inject restart index if this is not the first strip
		if(!indices.empty())
			indices.push_back(0xFFFFFFFF);

		face->used = true;
		indices.push_back(face->indices[0]);
		indices.push_back(face->indices[1]);
		if(face->nverts==4)
			indices.push_back(face->indices[3]);
		indices.push_back(face->indices[2]);

		unsigned triangle_count = face->nverts-2;
		while(1)
		{
			unsigned last_index = indices.back();
			unsigned prev_index = indices[indices.size()-2];
			if(triangle_count%2)
				swap(last_index, prev_index);

			// Try to find a face that uses the last two indices in the correct order
			vector<Face>::iterator next;
			unsigned i;
			for(next=faces.begin(); next!=faces.end(); ++next)
				if(!next->used)
				{
					for(i=0; i<next->nverts; ++i)
						if(next->indices[i]==prev_index)
							break;

					if(i>=next->nverts)
						continue;

					if(next->indices[(i+1)%next->nverts]==last_index)
						break;
				}

			// If there was no suitable face to continue this strip, break out
			if(next==faces.end())
				break;

			face = next;
			face->used = true;
			// For quadrilateral faces we have to emit two triangles
			if(next->nverts==4 && !(triangle_count%2))
				indices.push_back(face->indices[(i+3)%face->nverts]);
			indices.push_back(face->indices[(i+2)%face->nverts]);
			if(next->nverts==4 && (triangle_count%2))
				indices.push_back(face->indices[(i+3)%face->nverts]);

			triangle_count += face->nverts-2;
		}
	}

	set_data(vertices, indices);
}

void Object::render(const RenderState &state) const
{
	// Can't render anything without a shader
	if(!material || !material->get_shader())
		return;

	material->apply();

	Shader *shader = material->get_shader();
	shader->set_uniform("modelview", state.modelview_matrix);
	shader->set_uniform("projection", state.projection_matrix);
	shader->set_uniform("light_direction", state.light_direction);
	shader->set_uniform("light_intensity", state.light_intensity);
	shader->set_uniform("ambient_intensity", state.ambient_intensity);

	glBindVertexArray(vertex_array_id);
	glDrawElements(GL_TRIANGLE_STRIP, n_indices, GL_UNSIGNED_INT, 0);
}


VertexRef::VertexRef():
	vertex(-1),
	normal(-1),
	texcoord(-1)
{ }

bool VertexRef::operator<(const VertexRef &other) const
{
	if(vertex!=other.vertex)
		return vertex<other.vertex;
	else if(normal!=other.normal)
		return normal<other.normal;
	else
		return texcoord<other.texcoord;
}

istream &operator>>(istream &input, VertexRef &vref)
{
	string word;
	input >> word;
	if(!input)
		return input;

	vref = VertexRef();

	string::size_type slash = word.find('/');
	vref.vertex = strtol(word.substr(0, slash).c_str(), NULL, 10)-1;
	if(slash!=string::npos)
	{
		string::size_type slash2 = word.find('/', slash+1);
		if(slash2>slash+1)
			vref.texcoord = strtol(word.substr(slash+1, slash2-slash-1).c_str(), NULL, 10)-1;
		if(slash2!=string::npos)
			vref.normal = strtol(word.substr(slash2+1).c_str(), NULL, 10)-1;
	}

	return input;
}


Face::Face():
	nverts(0),
	used(false)
{ }

} // namespace SkrolliGL
