#version 330

in vec2 frag_pos;

uniform vec2 dimensions;

out vec4 frag_color;

float circle(vec2 c, float r) {
	return step(length(frag_pos - c) - r, r);
}

void main() {
	//vec3 color = vec3(1.);
	//color *= circle(vec2(0.), .1);
	//frag_color = vec4(color, 1.);
	frag_color = vec4(frag_pos, 0., 1.);
}
