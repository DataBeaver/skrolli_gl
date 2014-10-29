#version 150
uniform mat4 modelview;
uniform mat4 projection;
in vec4 in_position;
in vec3 in_normal;
in vec2 in_texcoord;
out vec3 v_normal;
out vec2 v_texcoord;
void main()
{
	gl_Position = projection*modelview*in_position;
	v_normal = mat3(modelview)*in_normal;
	v_texcoord = in_texcoord;
}
---
#version 150
uniform vec3 light_direction;
uniform float light_intensity;
uniform float ambient_intensity;
uniform sampler2D texture;
in vec3 v_normal;
in vec2 v_texcoord;
out vec4 out_color;
void main()
{
	float intensity = ambient_intensity+light_intensity*max(dot(normalize(v_normal), light_direction), 0.0);
	vec4 sample = texture2D(texture, v_texcoord);
	out_color = vec4(sample.rgb*intensity, sample.a);
}
