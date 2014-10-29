/*
Esimerkkiohjelma Skrollissa 2/2013 julkaistuun artikkeliin "OpenGL-ohjelmoinnin
perusteet".  Tämä versio käyttää GLFW:tä ikkunan avaamiseen ja GLEW:tä
funktioiden hakemiseen.
*/
#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>

using namespace std;

/* A structure describing a simple two dimensional vertex. */
struct Vertex
{
    float x, y;
};

/* Four vertices arranged as a rectangle.  The ordering is suitable for rendering as a triangle strip. */
static const Vertex vertices[] =
{
    { -0.5,  0.5 },
    { -0.5, -0.5 },
    {  0.5,  0.5 },
    {  0.5, -0.5 }
};

/* Handy constants for vertex attributes (there's only one for now). */
enum VertexAttribute
{
    POSITION
};

/* Source code for a simple vertex shader that uses input coordinates as is. */
static const char *vshader_src =
    "#version 140\n"
    "in vec4 in_position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = in_position;\n"
    "}";

/* Source code for a simple fragment shader that fills every pixel with a bright red color. */
static const char *fshader_src =
    "#version 140\n"
    "out vec4 out_color;\n"
    "void main()\n"
    "{\n"
    "   out_color = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}";

bool done = false;

int GLFWCALL window_closed()
{
	done = true;
	return true;
}

int main()
{
	// Initialize GLFW and open a window
	glfwInit();
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwOpenWindow(800, 600,
		0, 0, 0, 0,  // Red, green, blue, alpha bits
		0, 0,        // Depth, stencil bits
		GLFW_WINDOW);
	glfwSetWindowCloseCallback(window_closed);
	glfwSetWindowTitle("Skrolli GL");

	// Initialize GLEW
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

	// OpenGL code begins here
	const unsigned char *ver;
	ver = glGetString(GL_VERSION);
	cout<<"GL version: "<<ver<<endl;
	ver = glGetString(GL_SHADING_LANGUAGE_VERSION);
	cout<<"GLSL version: "<<ver<<endl;

	// Create a buffer and transfer the vertex data into it.
	unsigned vbuf_id;
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create a vertex array object to describe the vertex data.
	unsigned vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	glVertexAttribPointer(POSITION, 2, GL_FLOAT, false, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(POSITION);

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
	/* Bind input and output locations.  Must be done before linking the program.  This is not strictly necessary in such a simple program, but it's good practice for the future. */
	glBindAttribLocation(prog_id, POSITION, "in_position");
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

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	while(!done)
	{
		// Clear any previous contents from the framebuffer.
		glClear(GL_COLOR_BUFFER_BIT);
		// All the necessary bindings are still in effect from before.
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// Make the new contents visible.
		glfwSwapBuffers();

		int err = glGetError();
		if(err!=GL_NO_ERROR)
			cerr<<"OpenGL error: "<<hex<<err<<endl;
	}

	glfwTerminate();

	return 0;
}
