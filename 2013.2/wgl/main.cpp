/*
Esimerkkiohjelma Skrollissa 2/2013 julkaistuun artikkeliin "OpenGL-ohjelmoinnin
perusteet".  Tämä versio käyttää Windowsin matalan tason rajapintoja.
*/
#include <iostream>
#include <vector>
#include <windows.h>
#define GLCOREARB_PROTOTYPES
#include "glcorearb.h"
#include "wglext.h"

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

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	WNDCLASSEX wndcl;

	wndcl.cbSize = sizeof(WNDCLASSEX);
	wndcl.style = 0;
	wndcl.lpfnWndProc = &wndproc;
	wndcl.cbClsExtra = 0;
	wndcl.cbWndExtra = 0;
	wndcl.hInstance = hInstance;
	wndcl.hIcon = 0;
	wndcl.hCursor = LoadCursor(0, IDC_ARROW);
	wndcl.hbrBackground = 0;
	wndcl.lpszMenuName = 0;
	wndcl.lpszClassName = "skrolli_gl";
	wndcl.hIconSm = 0;

	RegisterClassEx(&wndcl);

	RECT rect;
	SetRect(&rect, 0, 0, 800, 600);
	int style = WS_OVERLAPPEDWINDOW&~WS_THICKFRAME;
	int exstyle = WS_EX_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, style, false, exstyle);

	HWND window = CreateWindowEx(exstyle,
		"skrolli_gl",
		"Window",
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right-rect.left, rect.bottom-rect.top,
		0,
		0,
		hInstance,
		0);

	HDC dc = GetDC(window);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;

	int pf_index = ChoosePixelFormat(dc, &pfd);
	if(!pf_index)
	{
		MessageBoxA(0, "Failed to find a doublebuffered RGBA pixelformat", "Initialization error", MB_OK|MB_ICONERROR);
		return 1;
	}
	SetPixelFormat(dc, pf_index, &pfd);

	HGLRC temp_context = wglCreateContext(dc);
	wglMakeCurrent(dc, temp_context);

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if(!wglCreateContextAttribsARB)
	{
		MessageBoxA(0, "Failed to find the function wglCreateContextAttribsARB.  Possibly OpenGL 3.2 is not supported?", "Initialization error", MB_OK|MB_ICONERROR);
		return 1;
	}

	int attribs[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	HGLRC context = wglCreateContextAttribsARB(dc, 0, attribs);

	wglMakeCurrent(dc, context);
	wglDeleteContext(temp_context);

	// wglGetProcAddress returns NULL for functions that existed in OpenGL 1.1
	PFNGLGENBUFFERSPROC glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	PFNGLBINDBUFFERPROC glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	PFNGLBUFFERDATAPROC glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	PFNGLCREATESHADERPROC glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	PFNGLSHADERSOURCEPROC glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	PFNGLCOMPILESHADERPROC glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	PFNGLGETSHADERIVPROC glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	PFNGLCREATEPROGRAMPROC glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	PFNGLATTACHSHADERPROC glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)wglGetProcAddress("glBindFragDataLocation");
	PFNGLLINKPROGRAMPROC glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	PFNGLGETPROGRAMIVPROC glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	PFNGLUSEPROGRAMPROC glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	// Create a buffer and transfer the vertex data into it.
	unsigned vbuf_id;
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	int err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	// Create a vertex array object to describe the vertex data.
	unsigned vao_id;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	glVertexAttribPointer(POSITION, 2, GL_FLOAT, false, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(POSITION);

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

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

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

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

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

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

	err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	// Use the program for rendering.
	glUseProgram(prog_id);

	// Set a black background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/*int*/ err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error during initialization: "<<hex<<err<<endl;

	ShowWindow(window, SW_SHOWNORMAL);

	while(!done)
	{
		// Handle events from the window system
		MSG msg;
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			DispatchMessage(&msg);

		// Clear any previous contents from the framebuffer.
		glClear(GL_COLOR_BUFFER_BIT);
		// All the necessary bindings are still in effect from before.
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// Make the new contents visible.
		SwapBuffers(dc);

		int err = glGetError();
		if(err!=GL_NO_ERROR)
			cerr<<"OpenGL error: "<<hex<<err<<endl;
	}

	ReleaseDC(window, dc);
	wglMakeCurrent(0, 0);
	wglDeleteContext(context);

	return 0;
}

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CLOSE:
		done = true;
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}
