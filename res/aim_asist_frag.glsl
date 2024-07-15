#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;


in vec2 frag_pos;

uniform vec2 player_pos;
uniform vec2 mouse_pos;
uniform float accuracy_thresh;

float view_cone(vec2 p, vec2 dir) {
	float cos_theta = accuracy_thresh;
	float cos_phi = dot(normalize(frag_pos - p), normalize(dir));

	return step(cos_theta, cos_phi);
}

float view_cone_striped(vec2 p, vec2 dir) {
	float cos_theta = accuracy_thresh;
	float cos_phi = dot(normalize(frag_pos - p), normalize(dir));

	float frag_dist = length(frag_pos - p);

	return step(cos_theta + sin(frag_dist) / 10., cos_phi);
}

void main() {
	vec3 color = vec3(1.);

	color *= view_cone_striped(player_pos, mouse_pos - player_pos);

	finalColor = vec4(color, .2);
}
