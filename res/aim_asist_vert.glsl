#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;

out vec2 fragTexCoord;
out vec4 fragColor;


uniform vec2 resolution;

out vec2 frag_pos;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    gl_Position = mvp * vec4(vertexPosition, 1.);

	// convert to coord system:
	// x E [0, width], y E [0, height], origin top-left, positive down-right
	// i need to change coord systems the proper way
	frag_pos = (vec2(gl_Position.x, -gl_Position.y) + vec2(1.)) / 2 * resolution;
}
