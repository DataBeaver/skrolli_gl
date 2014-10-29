#include <cmath>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <GL/glew.h>

using namespace std;

/* A structure describing a three dimensional vertex with a normal. */
struct Vertex
{
	float x, y, z;
	float nx, ny, nz;
};

/* Handy constants for vertex attributes. */
enum VertexAttribute
{
	POSITION,
	NORMAL
};

/* Source code for a vertex shader that computes a transformation and simple per-vertex lighting. */
static const char *vshader_src =
	"#version 140\n"
	"uniform mat4 modelview;\n"
	"uniform mat4 projection;\n"
	"in vec4 in_position;\n"
	"in vec3 in_normal;\n"
	"out float intensity;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection*modelview*in_position;\n"
	"	intensity = max((mat3(modelview)*in_normal).z, 0.0);\n"
	"}";

/* Source code for a fragment shader that uses input color and intensity to compute output color. */
static const char *fshader_src =
	"#version 140\n"
	"uniform vec4 color;\n"
	"in float intensity;\n"
	"out vec4 out_color;\n"
	"void main()\n"
	"{\n"
	"	out_color = vec4(color.rgb*intensity, color.a);\n"
	"}\n";

float frustum[] =
{
	 1.5f,  0.0f,  0.0f,  0.0f,
	 0.0f,  2.0f,  0.0f,  0.0f,
	 0.0f,  0.0f, -1.2f, -1.0f,
	 0.0f,  0.0f, -2.2f,  0.0f
};

template<typename T>
void set_attrib_array(unsigned index, unsigned size, float T::*member)
{
	T dummy;
	const char *offset_base = reinterpret_cast<const char *>(&dummy);
	unsigned offset = reinterpret_cast<const char *>(&(dummy.*member))-offset_base;
	glVertexAttribPointer(index, size, GL_FLOAT, false, sizeof(T), reinterpret_cast<void *>(offset));
	glEnableVertexAttribArray(index);
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window *window = SDL_CreateWindow("Skrolli GL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL);
	if(!window)
	{
		cerr<<"Could not open a window"<<endl;
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(!context)
	{
		cerr<<"Could not create OpenGL context"<<endl;
		return 1;
	}
	SDL_GL_MakeCurrent(window, context);

	int err = glewInit();
	if(err!=GLEW_OK)
	{
		cerr<<"GLEW initialization failed: "<<glewGetErrorString(err)<<endl;
		return 1;
	}
	if(!GLEW_VERSION_3_2)
	{
		cerr<<"OpenGL 3.2 not supported, even though we got a context for it?"<<endl;
		return 1;
	}

	const unsigned char *ver;
	ver = glGetString(GL_VERSION);
	cout<<"GL version: "<<ver<<endl;
	ver = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout<<"GLSL version: "<<ver<<endl;

	const unsigned rings = 32;
	const unsigned panels = 12;
	const float ring_radius = 2;
	const float tube_radius = 1;

	vector<Vertex> vertices;
	vertices.reserve(rings*panels);
	for(unsigned i=0; i<rings; ++i)
	{
		float angle_i = i*M_PI*2/rings;
		for(unsigned j=0; j<panels; ++j)
		{
			float angle_j = j*M_PI*2/panels;
			Vertex v;
			float r = ring_radius+cos(angle_j)*tube_radius;
			v.x = cos(angle_i)*r;
			v.y = sin(angle_i)*r;
			v.z = sin(angle_j)*tube_radius;
			v.nx = cos(angle_i)*cos(angle_j);
			v.ny = sin(angle_i)*cos(angle_j);
			v.nz = sin(angle_j);
			vertices.push_back(v);
		}
	}

	const unsigned strip_length = (panels+1)*2;

	vector<unsigned> indices;
	indices.reserve(rings*strip_length);
	for(unsigned i=0; i<rings; ++i)
		for(unsigned j=0; j<=panels; ++j)
		{
			indices.push_back(i*panels+(j%panels));
			indices.push_back(((i+1)%rings)*panels+(j%panels));
		}

	// Create a buffer and transfer the vertex data into it.
	unsigned vbuf_id;
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Ditto for index data.
	unsigned ibuf_id;
	glGenBuffers(1, &ibuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

	// Create a vertex array object to describe the vertex data.
	unsigned vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	set_attrib_array(POSITION, 3, &Vertex::x);
	set_attrib_array(NORMAL, 3, &Vertex::nx);

	// Element array buffer binding is part of vertex array object state.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf_id);

	// Create and compile a vertex shader.
	unsigned vsh_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsh_id, 1, &vshader_src, NULL);
	glCompileShader(vsh_id);
	// Check for compilation errors.
	int status;
	glGetShaderiv(vsh_id, GL_COMPILE_STATUS, &status);
	if(!status)
	{
		char buf[1024];
		glGetShaderInfoLog(vsh_id, sizeof(buf), NULL, buf);
		cerr<<"Shader compilation error:"<<endl<<buf<<endl;
		return -1;
	}

	// Create and compile a fragment shader.
	unsigned fsh_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsh_id, 1, &fshader_src, NULL);
	glCompileShader(fsh_id);
	// Check for compilation errors.
	glGetShaderiv(fsh_id, GL_COMPILE_STATUS, &status);
	if(!status)
	{
		char buf[1024];
		glGetShaderInfoLog(fsh_id, sizeof(buf), NULL, buf);
		cerr<<"Shader compilation error:"<<endl<<buf<<endl;
		return -1;
	}

	// Create a shader program and attach the shaders to it.
	unsigned prog_id = glCreateProgram();
	glAttachShader(prog_id, vsh_id);
	glAttachShader(prog_id, fsh_id);
	/* Bind input and output locations.  Must be done before linking the program. */
	glBindAttribLocation(prog_id, POSITION, "in_position");
	glBindAttribLocation(prog_id, NORMAL, "in_normal");
	glBindFragDataLocation(prog_id, 0, "out_color");
	// Link the shader program.
	glLinkProgram(prog_id);
	// Check for link errors.
	glGetProgramiv(prog_id, GL_LINK_STATUS, &status);
	if(!status)
	{
		char buf[1024];
		glGetProgramInfoLog(prog_id, sizeof(buf), NULL, buf);
		cerr<<"Shader link error:"<<endl<<buf<<endl;
		return -1;
	}    

	// Use the program for rendering.
	glUseProgram(prog_id);

	// Set a black background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	int color_loc = glGetUniformLocation(prog_id, "color");
	glUniform4f(color_loc, 0.4f, 0.8f, 0.0f, 1.0f);

	int modelview_loc = glGetUniformLocation(prog_id, "modelview");
	int projection_loc = glGetUniformLocation(prog_id, "projection");
	glUniformMatrix4fv(projection_loc, 1, false, frustum);

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	SDL_ShowWindow(window);

	bool done = false;
	while(!done)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type==SDL_QUIT)
				done = true;
		}

		// Clear any previous contents from the framebuffer.
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Initialize an identity matrix
		float modelview[16];
		for(unsigned i=0; i<16; ++i)
			modelview[i] = (i%5 ? 0.0f : 1.0f);

		// Move 8 units into the screen
		modelview[14] = -8.0f;

		// Apply rotation in the XZ plane
		float angle = SDL_GetTicks()/1000.0f;
		modelview[0] = cos(angle);
		modelview[8] = -sin(angle);
		modelview[2] = sin(angle);
		modelview[10] = cos(angle);

		glUniformMatrix4fv(modelview_loc, 1, false, modelview);

		// Render one triangle strips per ring.
		for(unsigned i=0; i<rings; ++i)
			glDrawElements(GL_TRIANGLE_STRIP, strip_length, GL_UNSIGNED_INT, reinterpret_cast<void *>(i*strip_length*sizeof(unsigned)));
		// Make the new contents visible.
		SDL_GL_SwapWindow(window);

		int err = glGetError();
		if(err!=GL_NO_ERROR)
			cerr<<"OpenGL error: "<<hex<<err<<endl;
	}

	SDL_Quit();

	return 0;
}
