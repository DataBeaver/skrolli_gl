/*
Esimerkkiohjelma Skrollissa 2/2013 julkaistuun artikkeliin "OpenGL-ohjelmoinnin
perusteet".  Tämä versio käyttää glX:ää, joka on monilla Unix-pohjaisilla
järjestelmillä toimiva matalan tason rajapinta.
*/
#include <iostream>
#include <vector>
#include <X11/Xlib.h>
#define GLCOREARB_PROTOTYPES
#include "glcorearb.h"
#include <GL/glx.h>

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

int main()
{
	Display *display = XOpenDisplay(0);

	int fbc_attribs[] =
	{
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, true,
		0
	};

	int count;
	GLXFBConfig *config = glXChooseFBConfig(display, DefaultScreen(display), fbc_attribs, &count);
	if(!config)
	{
		cerr<<"Could not find a doublebuffered RGBA FBConfig\n";
		return 1;
	}

	int ctx_attribs[] =
	{
		GLX_RENDER_TYPE, GLX_RGBA_TYPE,
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 2,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
	if(!glXCreateContextAttribsARB)
	{
		cerr<<"Could not find the function glXCreateContextAttribsARB."<<endl<<"Possibly OpenGL 3.2 is not supported?"<<endl;
		return 1;
	}

	GLXContext context = glXCreateContextAttribsARB(display, config[0], 0, true, ctx_attribs);

	XVisualInfo *visual = glXGetVisualFromFBConfig(display, config[0]);

	XSetWindowAttributes attr;
	// The colormap is needed or CreateWindow generates a BadMatch error
	attr.colormap = XCreateColormap(display, DefaultRootWindow(display), visual->visual, AllocNone);

	Window window = XCreateWindow(display,
		DefaultRootWindow(display),
		0, 0,      // Initial window position - usually the window manager overrides this
		800, 600,  // Initial window size
		0,         // Border width
		visual->depth,
		InputOutput,
		visual->visual,
		CWColormap, &attr);

	// Request notification when the window is closed
	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", true);
	XSetWMProtocols(display, window, &wm_delete_window, 1);

	GLXWindow glx_window = glXCreateWindow(display, config[0], window, 0);

	glXMakeContextCurrent(display, glx_window, glx_window, context);

	// These are not needed after initialization
	XFree(visual);
	XFree(config);

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
		return 1;
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
		return 1;
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
		return 1;
	}    

	// Use the program for rendering.
	glUseProgram(prog_id);

	// Set a black background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	int err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	XMapRaised(display, window);

	bool done = false;
	while(!done)
	{
		// Handle events from the window system
		int pending = XPending(display);
		for(; pending--;)
		{
			XEvent event;
			XNextEvent(display, &event);

			switch(event.type)
			{
			case ClientMessage:
				if(event.xclient.data.l[0]==(long)wm_delete_window)
					done = true;
				break;
			default:
				;  // Ignore any other events
			}
		}

		// Clear any previous contents from the framebuffer.
		glClear(GL_COLOR_BUFFER_BIT);
		// All the necessary bindings are still in effect from before.
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// Make the new contents visible.
		glXSwapBuffers(display, glx_window);

		int err = glGetError();
		if(err!=GL_NO_ERROR)
			cerr<<"OpenGL error: "<<hex<<err<<endl;
	}

	glXMakeContextCurrent(display, 0, 0, 0);
	glXDestroyWindow(display, glx_window);
	glXDestroyContext(display, context);
	XDestroyWindow(display, window);
	XCloseDisplay(display);

	return 0;
}
