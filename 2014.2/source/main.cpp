#include "bloom.h"
#include "camera.h"
#include "engine.h"
#include "group.h"
#include "object.h"
#include "instance.h"

using namespace std;
using namespace SkrolliGL;

class Controller: public EventListener
{
private:
	Camera &camera;
	float movement;
	float rotation;

public:
	Controller(Camera &);

	virtual void on_key_press(unsigned);
	virtual void on_key_release(unsigned);
	virtual void on_frame(float);
};

int main()
{
	Engine engine(960, 540);
	ResourceManager res_mgr;

	Camera camera;
	engine.set_camera(&camera);
	camera.set_aspect_ratio(16./9.);
	camera.set_position(Vector(10, -23, 1.7));
	camera.set_heading(120);
	camera.set_depth_range(0.1, 100);

	Controller controller(camera);
	engine.set_event_listener(&controller);

	engine.set_background_color(0.30, 0.47, 0.55);
	engine.set_light_direction(Vector(-0.8, -0.7, 1.3));
	engine.set_light_intensity(0.4);
	engine.set_ambient_intensity(0.2);

	res_mgr.load_directory("data");
	Group scene;
	scene.add(res_mgr.get<Group>("cottage.scene"));
	engine.set_scene_root(&scene);

	Instance blades(res_mgr.get<Object>("blades.obj"));
	blades.set_matrix(Matrix::translation(9, -0.6, 5.1)*Matrix::rotation_x(90));
	scene.add(blades);
	engine.rotate_animated_continuous(blades, 'Y', 90);

	Bloom bloom(960, 540);
	engine.add_postprocessor(bloom);

	while(engine.next_frame()) ;

	return 0;
}

Controller::Controller(Camera &c):
	camera(c),
	movement(0),
	rotation(0)
{ }

void Controller::on_key_press(unsigned k)
{
	if(k=='w')
		movement = 1;
	else if(k=='s')
		movement = -1;
	else if(k=='a')
		rotation = 1;
	else if(k=='d')
		rotation = -1;
}

void Controller::on_key_release(unsigned k)
{
	if(k=='w' || k=='s')
		movement = 0;
	else if(k=='a' || k=='d')
		rotation = 0;
}

void Controller::on_frame(float dt)
{
	camera.set_heading(camera.get_heading()+rotation*90*dt);
	float a = camera.get_heading()*M_PI/180;
	camera.set_position(camera.get_position()+Vector(cos(a), sin(a), 0)*(movement*5*dt));
}
