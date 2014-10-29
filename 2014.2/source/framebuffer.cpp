#include <GL/glew.h>
#include "framebuffer.h"

namespace SkrolliGL {

int Framebuffer::system_viewport[4] = { 0, 0, 0, 0 };

Framebuffer::Framebuffer(unsigned w, unsigned h):
	width(w),
	height(h),
	depth_buf_id(0)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	color_tex.create(width, height, Texture::RGB);
	color_tex.set_wrap(false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex.get_id(), 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id);
	if(depth_buf_id)
		glDeleteRenderbuffers(1, &depth_buf_id);
}

void Framebuffer::set_float(bool f)
{
	if(f)
		color_tex.create(width, height, Texture::RGB_FLOAT);
	else
		color_tex.create(width, height, Texture::RGB);
}

void Framebuffer::add_depth_buffer()
{
	if(depth_buf_id)
		return;

	glGenRenderbuffers(1, &depth_buf_id);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buf_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buf_id);
}

void Framebuffer::bind()
{
	// Store the system viewport so it can be restored later
	if(!system_viewport[2])
		glGetIntegerv(GL_VIEWPORT, system_viewport);

	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(system_viewport[0], system_viewport[1], system_viewport[2], system_viewport[3]);
}

} // namespace SkrolliGL
