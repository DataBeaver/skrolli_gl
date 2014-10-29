#version 150
uniform mat4 modelview;
uniform mat4 projection;
in vec4 in_position;
void main()
{
	gl_Position = projection*modelview*in_position;
}
---
#version 150
uniform vec4 color;
uniform float intensity;
out vec4 out_color;
void main()
{
	out_color = vec4(color.rgb*intensity, color.a);
}
