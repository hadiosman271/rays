#version 330

in vec3 vertexPosition;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matProjection;
uniform vec2 resolution;

out vec2 frag_pos;

void main() {
    gl_Position = mvp * vec4(vertexPosition, 1.);

	// converting coord system
	// i need to do this the proper way with a matrix
	frag_pos = (vec2(gl_Position.x, -gl_Position.y) + vec2(1.)) / 2 * resolution;
}
