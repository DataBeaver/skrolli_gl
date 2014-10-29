#ifndef SKROLLIGL_ENGINE_H_
#define SKROLLIGL_ENGINE_H_

#include <list>
#include <SDL.h>
#include "animation.h"

namespace SkrolliGL {

class Camera;
class EventListener;
class Instance;
class Renderable;

/*
High-level interface to the engine.  
*/
class Engine
{
private:
	SDL_Window *window;
	SDL_GLContext context;
	EventListener *listener;
	Vector light_direction;
	float light_intensity;
	float ambient_intensity;
	const Renderable *scene_root;
	const Camera *camera;
	std::list<Animation *> animations;
	unsigned last_frame;

public:
	Engine();
	Engine(unsigned, unsigned);
private:
	void init(unsigned, unsigned);
public:
	~Engine();

	/* Sets a listener that receives input and other events. */
	void set_event_listener(EventListener *);

	/* Set the color to fill the screen with before rendering. */
	void set_background_color(float, float, float);

	/* Sets the direction of global illumination.  The vector should point
	towards the light source. */
	void set_light_direction(const Vector &);

	void set_light_intensity(float);

	void set_ambient_intensity(float);

	/* Sets the scene to be rendered.  Usually this is an instance of Group. */
	void set_scene_root(const Renderable *);

	/* Sets the camera to be used for rendering.  The camera's projection and
	view matrices are passed to Renderables in RenderState. */
	void set_camera(const Camera *);

	void move_animated(Instance &, const Vector &, float, Animation::EasingType = Animation::CUBIC);

	void rotate_animated(Instance &, char, float, float, Animation::EasingType = Animation::CUBIC);

	void rotate_animated_continuous(Instance &, char, float);

	/* Cancels all animations affecting an Instance. */
	void cancel_animations(Instance &);

	/* Receives events and renders the next frame.  This should be called
	regularly from the main loop of the program.  Returns false if a quit event
	was received, true otherwise. */
	bool next_frame();
};


/*
Event listener interface for the Engine.  Derive a class from this and pass an
instance of it to Engine::set_event_listener to receive events.
*/
class EventListener
{
protected:
	EventListener() { }
public:
	virtual ~EventListener() { }

	virtual void on_quit() { }
	virtual void on_key_press(unsigned) { }
	virtual void on_key_release(unsigned) { }
	virtual void on_frame(float) { }
};

} // namespace SkrolliGL

#endif
