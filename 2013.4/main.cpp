#include <cmath>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <GL/glew.h>
#include "mathutils.h"
#include "object.h"
#include "shader.h"
#include "texture.h"

using namespace std;

/* Source code for a vertex shader that computes a transformation and vectors
for per-fragment lighting. */
static const char *textured_vertex_src =
	"#version 140\n"
	"uniform mat4 modelview;\n"
	"uniform mat4 projection;\n"
	"uniform vec3 light_direction;\n"
	"in vec4 in_position;\n"
	"in vec3 in_normal;\n"
	"in vec2 in_texcoord;\n"
	"out vec3 light;\n"
	"out vec3 normal;\n"
	"out vec2 texcoord;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection*modelview*in_position;\n"
	"	light = light_direction;\n"
	"	normal = mat3(modelview)*in_normal;\n"
	"	texcoord = in_texcoord;\n"
	"}";

/* Source code for a fragment shader that computes diffuse lighting for a
textured surface. */
static const char *textured_fragment_src =
	"#version 140\n"
	"uniform sampler2D texture;\n"
	"in vec3 light;\n"
	"in vec3 normal;\n"
	"in vec2 texcoord;\n"
	"out vec4 out_color;\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture2D(texture, texcoord);\n"
	"	float intensity = max(dot(normalize(light), normalize(normal)), 0.0);\n"
	"	out_color = vec4(color.rgb*(0.4+intensity*0.6), color.a);\n"
	"}\n";

/* Source code for a vertex shader that computes a transformation and vectors
for normalmapped per-fragment lighting. */
static const char *normalmap_vertex_src =
	"#version 140\n"
	"uniform mat4 modelview;\n"
	"uniform mat4 projection;\n"
	"uniform vec3 light_direction;\n"
	"in vec4 in_position;\n"
	"in vec3 in_normal;\n"
	"in vec2 in_texcoord;\n"
	"in vec3 in_tangent;\n"
	"in vec3 in_binormal;\n"
	"out vec2 texcoord;\n"
	"out vec3 tbn_light;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection*modelview*in_position;\n"
	"	mat3 normal_matrix = mat3(modelview);\n"
	"	mat3 tbn_matrix = normal_matrix*mat3(in_tangent, in_binormal, in_normal);\n"
	"	texcoord = in_texcoord;\n"
	"	tbn_light = light_direction*tbn_matrix;\n"
	"}";

/* Source code for a fragment shader that computes diffuse lighting for a
textured and normalmapped surface. */
static const char *normalmap_fragment_src =
	"#version 140\n"
	"uniform vec4 color;\n"
	"uniform sampler2D texture;\n"
	"uniform sampler2D normalmap;\n"
	"in vec2 texcoord;\n"
	"in vec3 tbn_light;\n"
	"out vec4 out_color;\n"
	"void main()\n"
	"{\n"
	"	vec4 color = texture2D(texture, texcoord);\n"
	"	vec3 normal = texture2D(normalmap, texcoord).rgb*2.0-1.0;\n"
	"	float intensity = max(dot(normalize(tbn_light), normal), 0.0);\n"
	"	out_color = vec4(color.rgb*(0.4+intensity*0.6), color.a);\n"
	"}\n";

SDL_Window *initialize_gl();
Object *create_ground(float);
Object *create_cube(float);
Object *create_torus(float, float, unsigned, unsigned);

int main()
{
	SDL_Window *window = initialize_gl();
	if(!window)
		// An error message was already printed
		return 1;

	// Create various resources
	Shader textured_shader(textured_vertex_src, textured_fragment_src);
	Shader normalmap_shader(normalmap_vertex_src, normalmap_fragment_src);

	Texture hextiles_color("hextiles-color.png");
	Texture hextiles_normal("hextiles-normal.png");
	// Texture from wikimedia user Bubba73
	Texture wood("wood.png");
	// Texture from wikimedia user temporalata
	Texture tiles_color("tiles-color.png");

	Object *ground_object = create_ground(20);
	ground_object->set_shader(&textured_shader);
	ground_object->set_texture(&tiles_color);

	Object *cube_object = create_cube(3);
	cube_object->set_shader(&textured_shader);
	cube_object->set_texture(&wood);

	Object *torus_object = create_torus(2, 1, 64, 32);
	torus_object->set_shader(&normalmap_shader);
	torus_object->set_texture(&hextiles_color, &hextiles_normal);
	torus_object->set_matrix(Matrix::translation(3, 1, 1.5));

	// Initialize camera location
	float camera_yaw = 0;
	float camera_pitch = -0.5;
	float camera_distance = 12;
	Matrix projection_matrix = Matrix::frustum(M_PI/3, 4.0/3.0, camera_distance*0.05, camera_distance*5.0);

	// Some uniform values won't change during execution, so set them here
	textured_shader.use();
	textured_shader.set_uniform("projection", projection_matrix);
	textured_shader.set_uniform("texture", 0);

	normalmap_shader.use();
	normalmap_shader.set_uniform("projection", projection_matrix);
	normalmap_shader.set_uniform("texture", 0);
	normalmap_shader.set_uniform("normalmap", 1);

	// Take note of the start time for animation
	unsigned start = SDL_GetTicks();

	bool done = false;
	while(!done)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type==SDL_QUIT)
				done = true;
		}

		unsigned now = SDL_GetTicks();

		// Animate the camera around the objects
		camera_yaw = (now-start)/4000.0;
		camera_pitch = sin(camera_yaw);
		camera_pitch *= camera_pitch;
		camera_pitch =-0.3-camera_pitch*0.5;

		Vector camera_position = Vector(-cos(camera_yaw)*cos(camera_pitch),
			-sin(camera_yaw)*cos(camera_pitch),
			-sin(camera_pitch))*camera_distance;
		Matrix view_matrix = Matrix::rotation_x(-camera_pitch-M_PI/2)*
			Matrix::rotation_z(M_PI/2-camera_yaw)*
			Matrix::translation(camera_position*-1);

		// Rotate the torus object around Z axis
		torus_object->set_matrix(Matrix::translation(3, 1, 1.5)*Matrix::rotation_z((now-start)/3000.0));

		// Rotate the cube object in a complicated way
		cube_object->set_matrix(Matrix::translation(-3, -1, 2.2)*Matrix::rotation_x(M_PI/6)*Matrix::rotation_y((now-start)/5000.0));

		// Clear any previous contents from the framebuffer.
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Render each object
		ground_object->render(view_matrix);
		torus_object->render(view_matrix);
		cube_object->render(view_matrix);

		// Make the new contents visible.
		SDL_GL_SwapWindow(window);

		int err = glGetError();
		if(err!=GL_NO_ERROR)
			cerr<<"OpenGL error: "<<hex<<err<<endl;
	}

	SDL_Quit();

	return 0;
}

SDL_Window *initialize_gl()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	// I'd like to create a forward-compatible context, but it breaks GLEW
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);*/

	SDL_Window *window = SDL_CreateWindow("Skrolli GL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL);
	if(!window)
	{
		cerr<<"Could not open a window"<<endl;
		return 0;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(!context)
	{
		cerr<<"Could not create OpenGL context"<<endl;
		return 0;
	}
	SDL_GL_MakeCurrent(window, context);

	int err = glewInit();
	if(err!=GLEW_OK)
	{
		cerr<<"GLEW initialization failed: "<<glewGetErrorString(err)<<endl;
		return 0;
	}
	if(!GLEW_VERSION_3_2)
	{
		cerr<<"OpenGL 3.2 not supported"<<endl;
		return 0;
	}

	// Set a black background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFFFFFF);

	err = glGetError();
	if(err!=GL_NO_ERROR)
	{
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;
		return 0;
	}

	SDL_ShowWindow(window);

	return window;
}

void create_plane(vector<Object::Vertex> &vertices, vector<unsigned> &indices, Vector origin, Vector side1, Vector side2)
{
	unsigned base = vertices.size();
	if(!indices.empty())
		indices.push_back(0xFFFFFFFF);

	Vector normal = side1.cross(side2);
	normal = normal*(1.0/normal.length());
	Vector tangent = side1*(1.0/side1.length());
	Vector binormal = side2*(1.0/side2.length());
	Object::Vertex v;
	v.nx = normal.x;
	v.ny = normal.y;
	v.nz = normal.z;
	v.tx = tangent.x;
	v.ty = tangent.y;
	v.tz = tangent.z;
	v.bx = binormal.x;
	v.by = binormal.y;
	v.bz = binormal.z;
	for(unsigned i=0; i<4; ++i)
	{
		v.u = i%2;
		v.v = i/2;
		Vector p = origin+side1*v.u+side2*v.v;
		v.x = p.x;
		v.y = p.y;
		v.z = p.z;
		vertices.push_back(v);
		indices.push_back(base+i);
	}
}

Object *create_ground(float size)
{
	vector<Object::Vertex> vertices;
	vector<unsigned> indices;
	create_plane(vertices, indices, Vector(-size/2, -size/2, 0), Vector(size, 0, 0), Vector(0, size, 0));
	return new Object(vertices, indices);
}

Object *create_cube(float size)
{
	vector<Object::Vertex> vertices;
	vector<unsigned> indices;
	create_plane(vertices, indices, Vector(-size/2, -size/2, size/2), Vector(size, 0, 0), Vector(0, size, 0));
	create_plane(vertices, indices, Vector(size/2, size/2, -size/2), Vector(-size, 0, 0), Vector(0, -size, 0));
	create_plane(vertices, indices, Vector(size/2, -size/2, -size/2), Vector(0, size, 0), Vector(0, 0, size));
	create_plane(vertices, indices, Vector(size/2, size/2, -size/2), Vector(-size, 0, 0), Vector(0, 0, size));
	create_plane(vertices, indices, Vector(-size/2, size/2, -size/2), Vector(0, -size, 0), Vector(0, 0, size));
	create_plane(vertices, indices, Vector(-size/2, -size/2, -size/2), Vector(size, 0, 0), Vector(0, 0, size));
	return new Object(vertices, indices);
}

Object *create_torus(float ring_radius, float tube_radius, unsigned rings, unsigned panels)
{
	vector<Object::Vertex> vertices;
	vertices.reserve((rings+1)*(panels+1));
	/* The vertices along the seams get duplicated to avoid texture wrapping
	artifacts */
	for(unsigned i=0; i<=rings; ++i)
	{
		float angle_i = i*M_PI*2/rings;
		for(unsigned j=0; j<=panels; ++j)
		{
			float angle_j = j*M_PI*2/panels;
			Object::Vertex v;
			float r = ring_radius+cos(angle_j)*tube_radius;
			v.x = cos(angle_i)*r;
			v.y = sin(angle_i)*r;
			v.z = sin(angle_j)*tube_radius;
			v.nx = cos(angle_i)*cos(angle_j);
			v.ny = sin(angle_i)*cos(angle_j);
			v.nz = sin(angle_j);
			v.u = i*1.0f/rings;
			v.v = j*1.0f/panels;
			v.tx = -sin(angle_i);
			v.ty = cos(angle_i);
			v.tz = 0;
			v.bx = cos(angle_i)*-sin(angle_j);
			v.by = cos(angle_i)*-sin(angle_j);
			v.bz = cos(angle_j);
			vertices.push_back(v);
		}
	}

	const unsigned strip_length = (panels+1)*2;

	vector<unsigned> indices;
	indices.reserve(rings*strip_length);
	for(unsigned i=0; i<rings; ++i)
	{
		// Start a new primitive for each ring
		if(i>0)
			indices.push_back(0xFFFFFFFF);
		for(unsigned j=0; j<=panels; ++j)
		{
			indices.push_back(i*(panels+1)+j);
			indices.push_back((i+1)*(panels+1)+j);
		}
	}

	return new Object(vertices, indices);
}
