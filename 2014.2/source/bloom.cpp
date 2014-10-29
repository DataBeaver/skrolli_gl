#include <stdexcept>
#include <sstream>
#include "bloom.h"

using namespace std;

const char *vshader =
	"#version 130\n"
	"in vec2 in_position;\n"
	"out vec2 texcoord;\n"
	"void main()\n"
	"{\n"
	"  gl_Position = vec4(in_position, 0.0, 1.0);\n"
	"  texcoord = (in_position+1.0)/2.0;\n"
	"}";

const char *blur_fshader = 
	"#version 130\n"
	"uniform sampler2D source;\n"
	"uniform vec2 delta;\n"
	"uniform float factors[19];\n"
	"in vec2 texcoord;\n"
	"out vec4 out_color;\n"
	"void main()\n"
	"{\n"
	"  out_color = vec4(0.0, 0.0, 0.0, 0.0);\n"
	"  for(int i=0; i<19; ++i)\n"
	"    out_color += texture2D(source, texcoord+delta*float(i-9))*factors[i];\n"
	"}";

const char *combine_fshader =
	"#version 130\n"
	"uniform sampler2D source;\n"
	"uniform sampler2D blurred;\n"
	"uniform float strength;\n"
	"in vec2 texcoord;\n"
	"out vec4 out_color;\n"
	"void main()\n"
	"{\n"
	"  out_color = mix(texture2D(source, texcoord), texture2D(blurred, texcoord), strength);\n"
	"}";

namespace SkrolliGL {

Bloom::Bloom(unsigned w, unsigned h):
	width(w),
	height(h),
	scaling(1.0f),
	scene_fbo(w, h),
	hblur_fbo(w, h),
	vblur_fbo(w, h)
{
	scene_fbo.set_float(true);
	hblur_fbo.set_float(true);
	vblur_fbo.set_float(true);

	// Create the shaders and prepare sampler uniforms
	blur_shader.set_source(vshader, blur_fshader);
	blur_shader.bind();
	blur_shader.set_uniform("source", 0);
	combine_shader.set_source(vshader, combine_fshader);
	combine_shader.bind();
	combine_shader.set_uniform("source", 0);
	combine_shader.set_uniform("blurred", 1);

	// Initialize the object with a simple rectangle
	vector<Object::Vertex> vertices;
	vector<unsigned> indices;
	for(unsigned i=0; i<4; ++i)
	{
		Object::Vertex v;
		v.x = (i/2)*2.0f-1.0f;
		v.y = 1.0f-(i%2)*2.0f;
		vertices.push_back(v);
		indices.push_back(i);
	}
	object.set_data(vertices, indices);
	object.set_material(&material);

	// Set some default parameters
	set_radius(5);
	set_strenght(0.3);
}

void Bloom::set_radius(float r)
{
	if(r<=0.0f)
		throw invalid_argument("Bloom::set_radius");

	vector<float> factors(19);
	// Compute a gaussian with standard deviation of r
	float sum = 0.0f;
	r = 2*r*r;
	for(int i=-9; i<=9; ++i)
		sum += (factors[9+i] = exp(-i*i/r));
	// Normalize the curve to avoid changes in brightness
	for(unsigned i=0; i<19; ++i)
		factors[i] /= sum;

	// Fill in the factors array in the blur shader
	blur_shader.bind();
	for(unsigned i=0; i<19; ++i)
	{
		ostringstream name;
		name << "factors["<<i<<"]";
		blur_shader.set_uniform(name.str(), factors[i]);
	}
}

void Bloom::set_scaling(float s)
{
	scaling = s;
}

void Bloom::set_strenght(float s)
{
	combine_shader.bind();
	combine_shader.set_uniform("strength", s);
}

void Bloom::render_effect(Framebuffer *target)
{
	// Horizontal blur
	blur_shader.bind();
	blur_shader.set_uniform("delta", scaling/width, 0.0f);
	material.set_shader(&blur_shader);
	material.set_texture(&scene_fbo.get_color_buffer());
	hblur_fbo.bind();
	// We need a dummy RenderState for the object
	RenderState state;
	object.render(state);

	// Vertical blur
	blur_shader.set_uniform("delta", 0.0f, scaling/height);
	material.set_texture(&hblur_fbo.get_color_buffer());
	vblur_fbo.bind();
	object.render(state);

	// Combine the blurred image with the original
	material.set_shader(&combine_shader);
	material.set_texture(&scene_fbo.get_color_buffer());
	vblur_fbo.get_color_buffer().bind(1);
	if(target)
		target->bind();
	else
		Framebuffer::unbind();
	object.render(state);
	Texture::unbind(1);
}

} // namespace SkrolliGL
