#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <SDL_image.h>
#include <GL/glew.h>
#include "camera.h"
#include "engine.h"
#include "framebuffer.h"
#include "postprocessor.h"
#include "renderable.h"
#include "rotationanimation.h"
#include "translationanimation.h"

using namespace std;

namespace SkrolliGL {

Engine::Engine()
{
	init(800, 600);
}

Engine::Engine(unsigned width, unsigned height)
{
	init(width, height);
}

void Engine::init(unsigned width, unsigned height)
{
	listener = 0;
	light_direction = Vector(0, 0, 1),
	light_intensity = 0.8f;
	ambient_intensity = 0.2f;
	scene_root = 0;
	camera = 0;
	last_frame = 0;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	// I'd like to create a forward-compatible context, but it breaks GLEW
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);*/

	window = SDL_CreateWindow("Skrolli GL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_OPENGL);
	if(!window)
		throw runtime_error("Could not open a window");

	context = SDL_GL_CreateContext(window);
	if(!context)
		throw runtime_error("Could not create OpenGL context");
	SDL_GL_MakeCurrent(window, context);

	int err = glewInit();
	if(err!=GLEW_OK)
	{
		ostringstream message;
		message << "GLEW initialization failed: " << glewGetErrorString(err);
		throw runtime_error(message.str());
	}
	/*if(!GLEW_VERSION_3_2)
		throw runtime_error("OpenGL 3.2 not supported");*/

	// Set a black background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFFFFFF);

	err = glGetError();
	if(err!=GL_NO_ERROR)
	{
		ostringstream message;
		message << "OpenGL error during initialization: " << hex << err;
		throw runtime_error(message.str());
	}

	SDL_ShowWindow(window);
}

Engine::~Engine()
{
	for(list<Animation *>::iterator i=animations.begin(); i!=animations.end(); ++i)
		delete *i;
}

void Engine::set_event_listener(EventListener *l)
{
	listener = l;
}

void Engine::set_background_color(float r, float g, float b)
{
	glClearColor(r, g, b, 0.0f);
}

void Engine::set_light_direction(const Vector &l)
{
	// Make sure the light direction is unit length
	light_direction = l*(1.0f/l.length());
}

void Engine::set_light_intensity(float i)
{
	light_intensity = i;
}

void Engine::set_ambient_intensity(float i)
{
	ambient_intensity = i;
}

void Engine::set_scene_root(const Renderable *r)
{
	scene_root = r;
}

void Engine::set_camera(const Camera *c)
{
	camera = c;
}

void Engine::move_animated(Instance &instance, const Vector &to_position, float duration, Animation::EasingType easing)
{
	animations.push_back(new TranslationAnimation(instance, to_position, duration, easing));
}

void Engine::rotate_animated(Instance &instance, char axis, float angle, float duration, Animation::EasingType easing)
{
	animations.push_back(new RotationAnimation(instance, axis, angle, duration, easing));
}

void Engine::rotate_animated_continuous(Instance &instance, char axis, float angle)
{
	animations.push_back(new RotationAnimation(instance, axis, angle));
}

void Engine::add_postprocessor(Postprocessor &pp)
{
	// Don't add the same postprocessor twice; move it to the end
	remove_postprocessor(pp);
	postprocessors.push_back(&pp);
}

void Engine::remove_postprocessor(Postprocessor &pp)
{
	list<Postprocessor *>::iterator i = find(postprocessors.begin(), postprocessors.end(), &pp);
	if(i!=postprocessors.end())
		postprocessors.erase(i);
}

bool Engine::next_frame()
{
	bool result = true;

	// Check for events
	SDL_PumpEvents();
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type==SDL_QUIT)
		{
			result = false;
			if(listener)
				listener->on_quit();
		}
		else if(event.type==SDL_KEYDOWN)
		{
			if(listener)
				listener->on_key_press(event.key.keysym.sym);
		}
		else if(event.type==SDL_KEYUP)
		{
			if(listener)
				listener->on_key_release(event.key.keysym.sym);
		}
	}

	// Run animations
	unsigned current_time = SDL_GetTicks();
	float since_last_frame = 0;
	if(last_frame)
		since_last_frame = (current_time-last_frame)/1000.0f;
	last_frame = current_time;

	for(list<Animation *>::iterator i=animations.begin(); i!=animations.end(); )
	{
		(*i)->advance(since_last_frame);
		if((*i)->has_finished())
		{
			delete *i;
			animations.erase(i++);
		}
		else
			++i;
	}

	if(listener)
		listener->on_frame(since_last_frame);

	// Prepare render target for the scene
	if(!postprocessors.empty())
	{
		Framebuffer &target = postprocessors.front()->get_render_target();
		target.add_depth_buffer();
		target.bind();
	}
	else
		Framebuffer::unbind();

	// Clear old contents from framebuffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Render the scene if we have one
	if(scene_root)
	{
		RenderState state;
		if(camera)
		{
			state.projection_matrix = camera->get_projection_matrix();
			state.modelview_matrix = camera->get_view_matrix();
			state.sky_direction = camera->get_view_matrix().transform_direction(Vector(0, 0, 1));
		}
		state.light_direction = state.modelview_matrix.transform_direction(light_direction);
		state.light_intensity = light_intensity;
		state.ambient_intensity = ambient_intensity;
		scene_root->render(state);
	}

	// Apply any postprocessors we may have
	for(list<Postprocessor *>::const_iterator i=postprocessors.begin(); i!=postprocessors.end(); ++i)
	{
		list<Postprocessor *>::const_iterator next = i;
		++next;
		Framebuffer *target = (next!=postprocessors.end() ? &(*next)->get_render_target() : 0);
		(*i)->render_effect(target);
	}

	// Make the new frame visible
	SDL_GL_SwapWindow(window);

	int err = glGetError();
	if(err!=GL_NO_ERROR)
		cerr<<"OpenGL error: "<<hex<<err<<endl;

	return result;
}

} // namespace SkrolliGL
