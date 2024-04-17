#include "raylib.h"
#include <math.h>

#define fclamp(x, a, b) fmin(fmax(x, a), b)

// Draws text and moves posX to the end of the text
void DrawAndMove(const char *text, int *posX, int posY, int fontSize, Color color) {
	DrawText(text, *posX, posY, fontSize, color);
	*posX += MeasureText(text, fontSize);
}

void DrawMouseButtonStats(const char *name, MouseButton button, int x, int y, int fontSize) {
	DrawAndMove(TextFormat("%s | ", name), &x, y, fontSize, RAYWHITE);
	DrawAndMove("Down"	  , &x, y, fontSize, IsMouseButtonDown(button) ? GREEN : RED);
	DrawAndMove(" | "	  , &x, y, fontSize, RAYWHITE);
	DrawAndMove("Up"	  , &x, y, fontSize, IsMouseButtonUp(button) ? GREEN : RED);
	DrawAndMove(" | "	  , &x, y, fontSize, RAYWHITE);
	DrawAndMove("Pressed" , &x, y, fontSize, IsMouseButtonPressed(button) ? GREEN : RED);
	DrawAndMove(" | "	  , &x, y, fontSize, RAYWHITE);
	DrawAndMove("Released", &x, y, fontSize, IsMouseButtonReleased(button) ? GREEN : RED);
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Drag and Drop Demo");
	SetTargetFPS(60);

	Camera2D camera = { .target = { 0, 0 }, .zoom = 1 };

	const int size = 200;
#define SQUARES 3
	Rectangle square[SQUARES] = {
		{ (screen.width - size) / 2, (screen.height - size) / 2, size, size },
		{ (screen.width - size) / 2 + 400, (screen.height - size) / 2, size, size },
		{ (screen.width - size) / 2 - 400, (screen.height - size) / 2, size, size }
	};

	int holding = -1;
	Vector2 holdPoint = { 0, 0 };

	while (!WindowShouldClose()) {
		Vector2 mouse = GetMousePosition();
		mouse.x = mouse.x / camera.zoom + camera.target.x;
		mouse.y = mouse.y / camera.zoom + camera.target.y;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			for (int i = SQUARES - 1; i >= 0; i--)
				if (CheckCollisionPointRec(mouse, square[i])) {
					holding = i;
					holdPoint.x = mouse.x - square[i].x;
					holdPoint.y = mouse.y - square[i].y;
				}
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			holding = -1;

		camera.zoom = fclamp(camera.zoom + GetMouseWheelMove() * 0.1, 0.1, 3);

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mouseDelta = GetMouseDelta();
			camera.target.x = fclamp(camera.target.x - mouseDelta.x / camera.zoom, -screen.width, screen.width);
			camera.target.y = fclamp(camera.target.y - mouseDelta.y / camera.zoom, -screen.height, screen.height);
		}	

		if (holding != -1) {
			square[holding].x = fclamp(mouse.x - holdPoint.x, 0, screen.width - square[holding].width);
			square[holding].y = fclamp(mouse.y - holdPoint.y, 0, screen.height - square[holding].height);
		}

		BeginDrawing();
			ClearBackground(BLACK);
			BeginMode2D(camera);
				for (int i = SQUARES - 1; i >= 0; i--) {
					DrawRectangleRec(square[i], GRAY);
					DrawRectangleLinesEx(square[i], 10, DARKGRAY);
				}

				for (int i = 0; i < SQUARES; i++) {
					Vector2 c1 = { square[i].x + square[i].width / 2, square[i].y + square[i].height / 2 };
					for (int j = 0; j < SQUARES; j++) {
						Vector2 c2 = { square[j].x + square[j].width / 2, square[j].y + square[j].height / 2 };
						DrawLineEx(c1, c2, 10, DARKGRAY);
					}
					DrawCircleV(c1, 10, DARKGRAY);
				}
				DrawRectangleLinesEx(screen, 5, DARKGRAY);
			EndMode2D();

			float padding = 5;
			int fontSize = 30;
	
			DrawMouseButtonStats("Left Mouse  ", MOUSE_BUTTON_LEFT  , padding, padding, fontSize);
			DrawMouseButtonStats("Right Mouse " , MOUSE_BUTTON_RIGHT , padding, padding + fontSize, fontSize);
			DrawMouseButtonStats("Middle Mouse", MOUSE_BUTTON_MIDDLE, padding, (padding + fontSize) * 2, fontSize);
 
			DrawText("Try dragging the squares", padding, screen.height - padding - fontSize, fontSize, RAYWHITE);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
