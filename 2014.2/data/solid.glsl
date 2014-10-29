#version 150
uniform mat4 modelview;
uniform mat4 projection;
in vec4 in_position;
in vec3 in_normal;
out vec3 v_normal;
void main()
{
	gl_Position = projection*modelview*in_position;
	v_normal = mat3(modelview)*in_normal;
}
---
#version 150
uniform vec3 light_direction;
uniform float light_intensity;
uniform float ambient_intensity;
uniform vec4 color;
in vec3 v_normal;
out vec4 out_color;
void main()
{
	float intensity = ambient_intensity+light_intensity*max(dot(normalize(v_normal), light_direction), 0.0);
	out_color = vec4(color.rgb*intensity, color.a);
}
