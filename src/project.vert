#version 450
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
in vec3 in_vertex;
in vec3 in_normal;
in vec2 in_texture;

out vec3 transfer_normal_transformed;
out vec4 transfer_vertex_transformed;
out vec2 transfer_texture;

void main(void)
{
	// Compute positon of vertex on screen
	mat4 global_transform = projectionMatrix*cameraMatrix*transformMatrix;
	vec4 vertex_transformed = global_transform*vec4(in_vertex, 1.0);
	gl_Position = vertex_transformed;
	
	// Transfer values to fragment shader
	transfer_normal_transformed = normalize(transpose(inverse(mat3(transformMatrix)))*in_normal);
	transfer_texture = in_texture;
	transfer_vertex_transformed = transformMatrix*vec4(in_vertex, 1.0);
	
}
