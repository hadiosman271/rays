#include "raylib.h"
#include "raymath.h"

void set_shader_vec2(Shader shader, const char *name, float x, float y) {
	SetShaderValue(shader, GetShaderLocation(shader, name),
		(float [2]) { x, y }, SHADER_UNIFORM_VEC2
	);
}

void set_shader_int(Shader shader, const char *name, int *i) {
	SetShaderValue(shader, GetShaderLocation(shader, name),
		i, SHADER_UNIFORM_INT
	);
}

Vector2 standard_to_polar(Vector2 p) {
	float r = Vector2Length(p);
	float theta = acos(p.x / r);
	return (Vector2) { r, p.y < 0 ? 2 * M_PI - theta : theta };
}

Vector2 screen_to_standard(Vector2 p, Vector2 s, Vector2 c) {
	p = Vector2Subtract(p, c);
	p.y *= -1;
	return Vector2Scale(p, 2. / s.y);
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	Vector2 screen_size = { screen.width, screen.height };
	InitWindow(screen.width, screen.height, "Coordinates");
	SetTargetFPS(60);

	Shader shader = LoadShader("res/coords_vert.glsl", "res/coords_frag.glsl");

	int polar = false;
	Vector2 c = { 0., 0. };

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_TAB)) {
			polar = !polar;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			c = Vector2Add(
				c,
				screen_to_standard(
					GetMouseDelta(),
					screen_size,
					(Vector2) { 0., 0. }
				)
			);
		}

		BeginDrawing();
			BeginShaderMode(shader);
				set_shader_vec2(shader, "resolution", screen.width, screen.height);
				set_shader_int(shader, "polar", &polar);
				set_shader_vec2(shader, "c", c.x, c.y);
				DrawRectangleRec(screen, WHITE);
			EndShaderMode();

			Vector2 mouse = GetMousePosition();
			Vector2 uv = Vector2Subtract(screen_to_standard(mouse, screen_size, Vector2Scale(screen_size, .5)), c);
			if (polar) {
				uv = standard_to_polar(uv);
			}
			DrawText(TextFormat("%g %g", uv.x, uv.y), mouse.x, mouse.y, 30, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
