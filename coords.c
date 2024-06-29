#include "raylib.h"

void set_shader_vec2(Shader shader, const char *name, float x, float y) {
	SetShaderValue(shader, GetShaderLocation(shader, name),
		(float [2]) { x, y }, SHADER_UNIFORM_VEC2
	);
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Coordinates");
	SetTargetFPS(60);

	Shader shader = LoadShader("res/vert.glsl", "res/frag.glsl");

	while (!WindowShouldClose()) {

		BeginDrawing();
			BeginShaderMode(shader);
				set_shader_vec2(shader, "resolution", screen.width, screen.height);
				DrawRectangleRec(screen, WHITE);
			EndShaderMode();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
