#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define BG_1 (Color) { 35,  41, 49, 255 }
#define BG_2 (Color) { 57,  62, 70, 255 }
#define NEW_GREEN (Color) { 78, 204, 163, 255 }

Rectangle points_bbox(Vector2 point[], int points) {
	Vector2 min = { 9999, 9999 };
	Vector2 max = { 0, 0 };
	for (int i = 0; i < points; i++) {
		if (point[i].x < min.x) min.x = point[i].x;
		if (point[i].y < min.y) min.y = point[i].y;
		if (point[i].x > max.x) max.x = point[i].x;
		if (point[i].y > max.y) max.y = point[i].y;
	}
	return (Rectangle) { min.x, min.y, max.x - min.x, max.y - min.y };
}

void draw_rounded_rect(int x, int y, int width, int height, Color color) {
	float r = 50;
	DrawRectangle(x + r, y, width - r * 2, height, color);
	DrawRectangle(x, y + r, r, height - r * 2, color);
	DrawRectangle(x + width - r, y + r, r, height - r * 2, color);
	DrawCircle(x + r, y + r, r, color);
	DrawCircle(x + r, y + height - r, r, color);
	DrawCircle(x + width - r, y + r, r, color);
	DrawCircle(x + width - r, y + height - r, r, color);
}

void draw_line_with_t(Vector2 p1, Vector2 p2, float t, float thick, Color color) {
	DrawLineEx(p1, Vector2Add(p1, Vector2Scale(Vector2Subtract(p2, p1), t)), thick, color);
}

void draw_bezier_with_t(Vector2 p0, Vector2 p1, Vector2 p2, float t, float thick, Color color) {
	float step = 1. / 32;

	float t_max = t;
	t = 0;
	Vector2 p = p0;
	while (t <= t_max) {
		Vector2 p_next = Vector2Add(
			Vector2Add(
				p1,
				Vector2Scale(Vector2Subtract(p0, p1), (1 - t) * (1 - t))
			),
			Vector2Scale(Vector2Subtract(p2, p1), t * t)
		);
		DrawLineEx(p, p_next, thick, color);
		t += step;
		p = p_next;
	}
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Bezier Curves");
	SetTargetFPS(60);

	Vector2 point[3] = {
		{ screen.width * .16, screen.height * .25 },
		{ screen.width * .50, screen.height * .60 },
		{ screen.width * .83, screen.height * .25 },
	};
	float radius = 10;

	Vector2 t_slider_min = { .x = screen.width * .05 };
	Vector2 t_slider_max = { .x = t_slider_min.x + screen.width * .33 };
	Vector2 t_slider = { .x = t_slider_min.x };
	t_slider.y = t_slider_min.y = t_slider_max.y = screen.height * .77;
	float slider_radius = 15;
	float t = 0.;

	int holding = -1;

	float val = 0.;

	while (!WindowShouldClose()) {
		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			for (int i = 0; i < 3; i++) {
				if (CheckCollisionPointCircle(mouse, point[i], radius)) {
					holding = i;
				} else if (CheckCollisionPointCircle(mouse, t_slider, radius)) {
					holding = 4;
				}
			}
		}
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			holding = -1;
		}
		if (holding != -1) {
			if (holding == 4) {
				t_slider.x = Clamp(mouse.x, t_slider_min.x, t_slider_max.x);
				t = (t_slider.x - t_slider_min.x) / (t_slider_max.x - t_slider_min.x);
				t = (int) (t * 100) / 100.;
			} else {
				point[holding] = Vector2Clamp(
					mouse,
					(Vector2) { screen.x, screen.y },
					(Vector2) { screen.width, screen.height * .69 }
				);
			}
		}


		BeginDrawing();	
			ClearBackground(BG_1);
			float padding = 30;
			//draw_rounded_rect(screen.x + padding, screen.height * .7, screen.width - padding * 2, screen.height * .3 - padding, BG_2);

			DrawText("t", t_slider_min.x - radius * 4, t_slider_min.y - radius * 2, radius * 4, NEW_GREEN);
			DrawText("0", t_slider_min.x - radius, t_slider_min.y + radius * 2, radius * 3, LIGHTGRAY);
			DrawText("1", t_slider_max.x - radius, t_slider_max.y + radius * 2, radius * 3, LIGHTGRAY);

			DrawLineEx(t_slider_min, t_slider_max, 10, LIGHTGRAY);
			DrawCircleV(t_slider_min, slider_radius * .75, LIGHTGRAY);
			DrawCircleV(t_slider_max, slider_radius * .75, LIGHTGRAY);

			DrawText(TextFormat("%.2f", t), t_slider.x - radius, t_slider.y - radius * 5, radius * 3, LIGHTGRAY);
			DrawCircleV(t_slider, slider_radius, NEW_GREEN);

			for (int i = 1; i < 3; i++) {
				DrawLineEx(point[i], point[i - 1], 5, DARKGRAY);
			}

			//draw_line_with_t(point[0], point[2], t, 7, LIGHTGRAY);
			draw_bezier_with_t(point[0], point[1], point[2], t, 7, LIGHTGRAY);

			for (int i = 0; i < 3; i++) {
				DrawCircleV(point[i], radius, LIGHTGRAY);
				DrawCircleV(point[i], radius - 3, GRAY);
			}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
