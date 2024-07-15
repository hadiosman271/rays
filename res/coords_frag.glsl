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
uniform bool polar;
uniform vec2 c;

float circle(vec2 uv, float r) {
	return step(dot(uv, uv), r * r);
}

float cartesian_grid(vec2 uv, float w) {
	uv -= c;
	// lower multiple
	vec2 low = vec2(
		int(uv.x / w) * w, // closest multiple of w less than x
		int(uv.y / w) * w
	);
	vec2 d = vec2(
		uv.x >= 0 ?
			min(uv.x - low.x, low.x + w - uv.x) :  // when x < 0, the lower multiple becomes the
			min(low.x - uv.x, uv.x - (low.x - w)), // higher multiple, so i need to subtract w
		uv.y >= 0 ?
			min(uv.y - low.y, low.y + w - uv.y) :
			min(low.y - uv.y, uv.y - (low.y - w))
	);

	vec2 th = vec2(w / 100.);
	float br = .4;

	// highlight axis lines
	vec2 o = vec2(0., 0.);
	float th_f = 2.;
	if (abs(uv.x - o.x * w) < th.x * th_f) {
		th.x *= th_f;
		br = .6;
	}
	if (abs(uv.y - o.x * w) < th.y * th_f) {
		th.y *= th_f;
		br = .6;
	}

	return br * clamp(step(d.x, th.x) + step(d.y, th.y), 0., 1.);
}

#define PI 3.14159265358979323846

float polar_grid(vec2 uv, float w0) {
	// convert to polar
	float r = length(uv);
	float theta = acos(uv.x / r);
	uv = vec2(r, uv.y < 0 ? 2 * PI - theta : theta);

	vec2 w = vec2(w0, w0 * PI);
	vec2 low = vec2(
		int(uv.r / w.r) * w.r,
		int(uv.t / w.t) * w.t
	);
	vec2 d = vec2(
		min(uv.r - low.r, low.r + w.r - uv.r),
		min(uv.t - low.t, low.t + w.t - uv.t)
	);

	vec2 th = vec2(w0 / 100.);
	float br = .4;

	// highlight lines
	//vec2 o = vec2(4., 8.);
	//float th_f = 4.;
	//if (abs(uv.r - o.r * w.r) < th.r * th_f) {
	//	th.r *= th_f;
	//	br = .8;
	//}
	//if (abs((uv.t - o.t * w.t) * uv.r) < th.t * th_f) {
	//	th.t *= th_f;
	//	br = .8;
	//}

	return br * clamp(step(d.r, th.r) + step(d.t, th.t / uv.r), 0., 1.);
}

void main() {
    vec3 color = fragColor.rgb;
	vec2 uv = frag_pos;
	uv.x *= resolution.x / resolution.y;

	uv -= c;

	if (polar) {
		color *= polar_grid(uv, .2);
	} else {
		color *= cartesian_grid(uv, .2);
	}

    finalColor = vec4(color, fragColor.a);
}
