#version 450
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 transformMatrix;
uniform vec3 cameraPosition;
uniform vec3 cameraOrientation;

uniform vec3 lightSources[400]; // Not yet working
uniform int lightCount; // Not yet working
uniform bool bumpMap; // Do we use a bump_map

uniform sampler2D texUnit; // Texture color
uniform sampler2D bumpUnit; // Texture normal

in vec3 transfer_normal;
in vec4 transfer_vertex;
in vec2 transfer_texture;

out vec4 out_Color;

void main(void)
{	
	
	// Light color
	//vec3 light_level = vec3(1.0, 0.95, 0.75);
	vec3 light_level = vec3(1.0);
	// Use the correct normal vectors
	vec3 transfer_normal_transformed = transfer_normal;
	if (bumpMap)
	{
		vec3 new_normal = 2*vec3(texture(bumpUnit, transfer_texture)) - vec3(1, 1, 1);
		transfer_normal_transformed = normalize(transpose(inverse(mat3(transformMatrix)))*new_normal);
	}
	

	// Define reflectivity and specular exponent of material
	float specularExponent = 30;
	vec3 reflectivity = vec3(1);

	vec4 total_light = vec4(0);
	
	// idealy use lights given from uniform
	int lightCount = 3; 
	vec3 lightSources[] = {cameraPosition, vec3(1.5, 0.5, 1.5), vec3(10.5, 0.5, 10.5)};

	for (int i = 0; i < lightCount; ++i) // For each light
	{
		vec3 light_transformed = lightSources[i];
		
		vec3 vertex_to_light = normalize(light_transformed - vec3(transfer_vertex));
		
		// Used for attenuation (we give a higer attenuation for fixed light sources so it doeesn't bleed trgough walls)
		float distance = length(light_transformed - vec3(transfer_vertex));
		distance = distance*distance;
		if (i != 0) distance = distance*distance;

		// Ambiant lighting, only for the moving light
		vec3 ambiant = vec3(0.);
		if (i == 0) ambiant = vec3(0.1);
		
		// Diffuse component
		float costheta = dot(transfer_normal_transformed,vertex_to_light);
		
		vec3 diffuse = vec3(max(0, reflectivity.x*light_level.x*costheta), 
							max(0, reflectivity.y*light_level.y*costheta),
							max(0, reflectivity.z*light_level.z*costheta));

		
		// Specular component
		vec3 r = normalize(2*normalize(transfer_normal_transformed)*dot(vertex_to_light, normalize(transfer_normal_transformed)) - vertex_to_light);
		vec3 vertex_to_camera = normalize(cameraPosition - vec3(transfer_vertex));
		vec3 specular = reflectivity*light_level*pow(max(0,dot(r, vertex_to_camera)), specularExponent);

		// Attenutation
		float attenuation = (1.0 / (1.0 + (0.05 * distance * distance)));

		// Spotlight effect
		int spotlight_angle = 30;
		if (i == 0)
		{
			float lightToSurfaceAngle = degrees(acos(dot(-vertex_to_light, normalize(cameraOrientation))));
			if(lightToSurfaceAngle > 30){
				attenuation/= sqrt((lightToSurfaceAngle - spotlight_angle)/2.0 + 1);
			}
		}

		total_light += vec4(attenuation*(ambiant + diffuse + specular), 1.0);
	}
	vec4 texture = texture(texUnit, transfer_texture);
	
	// Mix the light with the texture
	out_Color = vec4(texture.x*total_light.x, texture.y*total_light.y, texture.z*total_light.z, texture.w*total_light.w); 

}
