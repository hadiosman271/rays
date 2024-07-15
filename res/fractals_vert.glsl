#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// Custom variables
uniform vec2 resolution;

out vec2 frag_pos;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    gl_Position = mvp * vec4(vertexPosition, 1.);

	frag_pos = gl_Position.xy * resolution;
}
