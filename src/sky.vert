#version 150
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
in vec3 in_vertex;
in vec3 in_normal;
in vec2 in_texture;


out vec2 transfer_texture;

void main(void)
{
	// Compute positon of vertex on screen
	gl_Position = projectionMatrix*cameraMatrix*transformMatrix*vec4(in_vertex, 1.0);
	
	// Transfer values to fragment shader
	transfer_texture = in_texture;

	
}
