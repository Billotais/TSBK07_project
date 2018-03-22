#version 150
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
in vec3 in_vertex;
in vec3 in_normal;
in vec2 in_texture;

out vec3 transfer_normal;
out vec4 transfer_vertex_transformed;
out vec2 transfer_texture;

void main(void)
{
	// Compute positon of vertex on screen
	vec4 vertex_transformed = projectionMatrix*cameraMatrix*transformMatrix*vec4(in_vertex, 1.0);
	gl_Position = vertex_transformed;
	
	// Transfer values to fragment shader
	transfer_normal = in_normal;
	transfer_texture = in_texture;
	transfer_vertex_transformed = vertex_transformed;
	
}
