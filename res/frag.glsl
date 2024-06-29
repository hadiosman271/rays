#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom variables
in vec2 frag_pos;
uniform vec2 resolution;

float circle(vec2 uv, vec2 c, float r) {
	return step(dot(uv, uv), r * r);
}

void main() {
    vec3 color = fragColor.rgb;
	vec2 uv = frag_pos;
	uv.x *= resolution.x / resolution.y;

    color *= circle(uv, vec2(0.), .5);

    finalColor = vec4(color, fragColor.a);
}
