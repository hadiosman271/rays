#include "raylib.h"

int main() {
	Rectangle screen = { 0, 0, 1920, 1080 };
	SetTargetFPS(60);
	InitWindow(screen.width, screen.height, "Graph");

	while (!WindowShouldClose()) {

		BeginDrawing();
			ClearBackground(DARKGRAY);
			DrawText("Hello, World!", screen.width / 2 - 300, screen.height / 2 - 100, 100, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
