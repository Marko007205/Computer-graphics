#version 330 core

out vec4 FragColour;

in vertex_out {
	vec3 FragColour;
} FragmentIn;

uniform int moonflag;

void main() {
	if (moonflag == 1) {
		FragColour = vec4(0.99f, 0.98f, 0.84f, 1.0f);
	} else {
		FragColour = vec4(FragmentIn.FragColour, 1.0f);
	}
}