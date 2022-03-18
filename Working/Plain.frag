#version 450

uniform sampler2D screen;

in vec2 UVs;

out vec4 FragColour;

void main()
{
	FragColour = texture(screen, UVs);
}