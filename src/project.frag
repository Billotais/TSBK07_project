#version 150
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;

uniform sampler2D texUnit;

in vec3 transfer_normal;
in vec4 transfer_vertex_transformed;
in vec2 transfer_texture;

out vec4 out_Color;

void main(void)
{
	// Define a light, might be passed as a parameter
	vec3 light_coord = cameraMatrix[0].xyz;
	vec3 light_level = vec3(1.0, 1.0, 1.0);
	bool isDirectional = false;
	

	// Define reflectivity and specular exponent of material
	float specularExponent = 10;
	vec3 reflectivity = vec3(0.4, 0.4, 0.4);

	// Light coordinates in new coordinate system
	vec3 light_transformed = vec3(projectionMatrix*cameraMatrix*vec4(light_coord, 1.0));

	// Define lght vector depending on the type of light
	vec3 vertex_to_light;
	if (isDirectional) 
		vertex_to_light = normalize(light_transformed);
	else 
		vertex_to_light = normalize(light_transformed - vec3(transfer_vertex_transformed));
	
	// Ambiant lighting
	vec3 ambiant = vec3(0.1, 0.1, 0.1);
	
	// Diffuse component
	float costheta = dot(normalize(transfer_normal),vertex_to_light);
	vec3 diffuse = vec3(max(0, reflectivity.x*light_level.x*costheta), 
						max(0, reflectivity.y*light_level.y*costheta),
						max(0, reflectivity.z*light_level.z*costheta));

	// Specular component
	vec3 r = normalize(2*normalize(transfer_normal*(dot(vertex_to_light, normalize(transfer_normal)))) - vertex_to_light);
	vec3 vertex_to_camera = normalize(vec3(projectionMatrix*cameraMatrix*cameraMatrix[0]) - vec3(transfer_vertex_transformed));
	vec3 specular = vec3(reflectivity.x*light_level.x*pow(max(0,dot(r, vertex_to_camera)), specularExponent),
						 reflectivity.y*light_level.y*pow(max(0,dot(r, vertex_to_camera)), specularExponent),
						 reflectivity.z*light_level.z*pow(max(0,dot(r, vertex_to_camera)), specularExponent));

	vec4 texture = texture(texUnit, transfer_texture);
	vec4 total_light = vec4(ambiant /*+ diffuse*/ + specular, 1.0);
	out_Color = vec4(texture.x*total_light.x, texture.y*total_light.y, texture.z*total_light.z, texture.w*total_light.w); 
}
