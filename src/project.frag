#version 450
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
uniform vec3 cameraPosition;

uniform sampler2D texUnit;
uniform sampler2D bumpUnit;

in vec3 transfer_normal_transformed;
in vec4 transfer_vertex_transformed;
in vec2 transfer_texture;

out vec4 out_Color;

void main(void)
{	
	
	
	// Define a light, might be passed as a parameter
	vec3 light_coord = vec3(10.5, 2, 10.5);
	//vec3 light_coord = vec3(10, 3, 10);
	vec3 light_level = vec3(1.0, 0.95, 0.75);
	
	vec3 new_normal = 2*vec3(texture(bumpUnit, transfer_texture)) - vec3(1, 1, 1);
	vec3 transfer_normal_transformed = normalize(transpose(inverse(mat3(transformMatrix)))*new_normal);

	// Define reflectivity and specular exponent of material
	float specularExponent = 30;
	vec3 reflectivity = vec3(1);

	// Light coordinates in new coordinate system
	vec3 light_transformed = cameraPosition;

	// Define lght vector depending on the type of light
	
	vec3 vertex_to_light = normalize(light_transformed - vec3(transfer_vertex_transformed));
	// Used for attenuation
	float distance = length(light_transformed - vec3(transfer_vertex_transformed));
	
	// Ambiant lighting
	vec3 ambiant = vec3(0.3);
	
	// Diffuse component
	
	float costheta = dot(transfer_normal_transformed,vertex_to_light);
	
	vec3 diffuse = vec3(max(0, reflectivity.x*light_level.x*costheta), 
						max(0, reflectivity.y*light_level.y*costheta),
						max(0, reflectivity.z*light_level.z*costheta));

	
	// Specular component
	vec3 r = normalize(2*normalize(transfer_normal_transformed)*dot(vertex_to_light, normalize(transfer_normal_transformed)) - vertex_to_light);
	vec3 vertex_to_camera = normalize(cameraPosition - vec3(transfer_vertex_transformed));

	vec3 specular = reflectivity*light_level*pow(max(0,dot(r, vertex_to_camera)), specularExponent);
	vec4 texture = texture(texUnit, transfer_texture);
	vec4 total_light = vec4((1.0 / (1.0 + (0.05 * distance * distance)))*(ambiant + diffuse + specular), 1.0);
	out_Color = vec4(texture.x*total_light.x, texture.y*total_light.y, texture.z*total_light.z, texture.w*total_light.w); 

}
