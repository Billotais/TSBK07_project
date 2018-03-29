#version 150


uniform sampler2D texUnit;

in vec2 transfer_texture;

out vec4 out_Color;

void main(void)
{	
	out_Color = 0.4*texture(texUnit, transfer_texture);
}
