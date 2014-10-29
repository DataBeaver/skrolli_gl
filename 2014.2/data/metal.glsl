#version 150
uniform mat4 modelview;
uniform mat4 projection;
in vec4 in_position;
in vec3 in_normal;
out vec3 v_normal;
out vec3 v_incident;
void main()
{
	vec4 eye_vertex = modelview*in_position;
	gl_Position = projection*eye_vertex;
	v_normal = mat3(modelview)*in_normal;
	v_incident = eye_vertex.xyz;
}
---
#version 150
uniform vec3 light_direction;
uniform float light_intensity;
uniform float ambient_intensity;
uniform vec3 sky_direction;
uniform vec4 color;
in vec3 v_normal;
in vec3 v_incident;
out vec4 out_color;
void main()
{
	vec3 n_normal = normalize(v_normal);
	vec3 reflected = reflect(normalize(v_incident), n_normal);
	float intensity = ambient_intensity+light_intensity*max(dot(n_normal, light_direction), 0.0);
	out_color = vec4(color.rgb*intensity+light_intensity*0.2*step(0.0, dot(reflected, sky_direction)), color.a);
}
