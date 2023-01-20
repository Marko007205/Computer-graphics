#version 330 core

out vec4 FragColour;
in vec2 TexCoords;

uniform sampler2D uTexture; 
uniform float uInterpolationFactor;

void main() {
	vec4 texColor = texture(uTexture, TexCoords);
	FragColour = texColor;
}