#ifndef SKROLLIGL_ENGINE_H_
#define SKROLLIGL_ENGINE_H_

#include <list>
#include <SDL.h>
#include "animation.h"

namespace SkrolliGL {

class Camera;
class EventListener;
class Instance;
class Postprocessor;
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
	std::list<Postprocessor *> postprocessors;

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

	/* Sets the intensity of global directional illumination.  Contributes to
	the brightness of surfaces that are facing the light. */
	void set_light_intensity(float);

	/* Sets the global ambient illumination intensity.  Contributes to the
	brightness of all surfaces. */
	void set_ambient_intensity(float);

	/* Sets the scene to be rendered.  Usually this is an instance of Group. */
	void set_scene_root(const Renderable *);

	/* Sets the camera to be used for rendering.  The camera's projection and
	view matrices are passed to Renderables in RenderState. */
	void set_camera(const Camera *);

	/* Smoothly moves an instance to a target location. */
	void move_animated(Instance &, const Vector &to, float duration, Animation::EasingType = Animation::CUBIC);

	/* Smoothly rotates an instance by a fixed angle.  Axis must be one of 'X',
	'Y', 'Z'. */
	void rotate_animated(Instance &, char axis, float angle, float duration, Animation::EasingType = Animation::CUBIC);

	/* Rotates an instance indefinitely at the given rate.  Axis must be one of
	'X', 'Y', 'Z'. */
	void rotate_animated_continuous(Instance &, char axis, float rate);

	/* Cancels all animations affecting an Instance. */
	void cancel_animations(Instance &);

	/* Appends a postprocessor to be applied after the scene has been rendered.
	Postprocessors are applied in the order they are added.  If the same
	postprocessor is added again, it's moved to the end of the list. */
	void add_postprocessor(Postprocessor &);

	void remove_postprocessor(Postprocessor &);

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

	/* Called just before rendering each frame.  The number of seconds elapsed
	since the previous frame is passed as an argument. */
	virtual void on_frame(float) { }
};

} // namespace SkrolliGL

#endif
