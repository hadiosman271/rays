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
uniform vec2 in_z_0;
uniform vec2 in_c;
uniform bool plot_c;
uniform int max_iters;

// f(z, c) = z^2 + c
vec2 f(vec2 z, vec2 c) {
	// (a + bi)(a + bi) == a^2 + 2abi + b^2i^2 == a^2 - b^2 + 2abi
	return vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
}

int mandelbrot(vec2 z_0, vec2 c) {
	vec2 z_n = z_0;
	int i;
	for (i = 0; i < max_iters && length(z_n) <= 2.; i++) {
		z_n = f(z_n, c);
	}
	return i;

	// remove color banding
	//return i + 1 - log(log(dot(z_n, z_n))) / log(2);
}

// Convert HEX color, to vec3 color
vec3 hexToColor(int hex) {
    float r = float((hex >> 16) & 0xFF) / 255.0;
    float g = float((hex >> 8) & 0xFF) / 255.0;
    float b = float(hex & 0xFF) / 255.0;
    return vec3(r, g, b);
}

// Function to generate a color from a distance
vec3 palette(float dist) {
	// Palette colors
    vec3 interior = vec3(0.);
	vec3 exterior[4];
	exterior[0] = hexToColor(0x2247f0);
	exterior[1] = hexToColor(0x22d8f0);
	exterior[2] = hexToColor(0xdced24);
	exterior[3] = hexToColor(0xed2424);

	// Return the interior palette colors if dist is 1.0
	if (dist == 1.) {
		return interior;
	}

	// Calculate the index for interpolation
	float index = mod(dist * 2., 2.);
	int i = int(floor(index));
	float t = fract(index);

	// Blend between interior and exterior colors based on distance
	vec3 color = mix(exterior[i], exterior[i + 1], t);
	return color;
}

void main() {
	vec3 color = vec3(1.);

	float iters = 0;
	if (plot_c) {
		iters = mandelbrot(in_z_0, frag_pos);
	} else {
		iters = mandelbrot(frag_pos, in_c);
	}

    // Calculate color based on iterations
    float s = pow(iters / float(max_iters), 40.0 / float(max_iters));
    color = palette(s);

	finalColor = vec4(color, 1.);
}
