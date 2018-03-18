#version 150
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
in vec3 in_vertex;
in vec3 in_normal;
in vec3 in_texture;

void main(void)
{
	gl_Position = vec4(in_vertex, 1.0);
}
