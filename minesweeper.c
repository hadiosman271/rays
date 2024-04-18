#include "raylib.h"

#include "positioning.h"
#include "minesweeper.h"

#define COL_1 (Color) { 17,  79,  175, 255 }
#define COL_2 (Color) { 0 ,  95,  14,  255 }
#define COL_3 (Color) { 147, 16,  28,  255 }
#define COL_4 (Color) { 0,   45,  117, 255 }
#define COL_5 (Color) { 102, 4,   22,  255 }
#define COL_6 (Color) { 20,  103, 127, 255 }
#define COL_7 (Color) { 28,  24,  45,  255 }
#define COL_8 (Color) { 145, 145, 145, 255 }
#define COL_BG (Color)     { 30,  30,  30,  255 }
#define COL_HIDDEN (Color) { 58,  58,  58,  255 }
#define COL_CLEAR (Color)  { 103, 103, 103, 255 }

#define ROWS 16
#define COLS 30
#define BOMBS 99

void draw_field(Cell field[], int rows, int cols, float cell_width, Rectangle bbox);

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Minesweeper");
	SetTargetFPS(60);

	Minesweeper ms = {
		.rows = ROWS, .cols = COLS, .bombs = BOMBS,
		.initialized = false, .game_over = false
	};
	ms.field = calloc(1, sizeof(Cell [ms.rows * ms.cols]));

	Rectangle field_bbox = get_bbox(
		(Vector2) { screen.width * .9, screen.width * .9 / COLS * ROWS },
		(Vector2) { 0, 20 },
		screen,
		(Anchor) { A_INSIDE, A_YCENTER, A_XCENTER }
	);
	float cell_width = field_bbox.width / COLS;

	while (!WindowShouldClose()) {
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse = GetMousePosition();
			int x = (mouse.x - field_bbox.x) / cell_width;
			int y = (mouse.y - field_bbox.y) / cell_width;
			clear_cell(&ms, x, y);
		}

		BeginDrawing();
			ClearBackground(COL_BG);
			draw_field(ms.field, ROWS, COLS, cell_width, field_bbox);

			if (ms.game_over) {
				// TODO: game over screen
			}
		EndDrawing();
	}

	free(ms.field);
	CloseWindow();
	return 0;
}

void draw_field(Cell field[], int rows, int cols, float cell_width, Rectangle bbox) {
	for (int i = COLS - 1; i >= 0; i--) {
		for (int j = 0; j < rows; j++) {
			Cell cell = field[j * cols + i];
			Rectangle cell_start = { bbox.x + cell_width * i, bbox.y + cell_width * j, cell_width, cell_width };
	
			if (cell.clear) {
				Color color;
				switch (cell.number) {
					case 1: color = COL_1; break;
					case 2: color = COL_2; break;
					case 3: color = COL_3; break;
					case 4: color = COL_4; break;
					case 5: color = COL_5; break;
					case 6: color = COL_6; break;
					case 7: color = COL_7; break;
					case 8: color = COL_8; break;
					default: color = BLACK; break;
				}
	
				DrawRectangleRec(cell_start, COL_CLEAR);
				if (cell.number > 0) {
					DrawText((const char []){ cell.number + '0', '\0' }, cell_start.x + 20, cell_start.y + 10, 50, color);
				} else if (cell.bomb) {
					DrawRectangleRec(cell_start, COL_3);
					DrawText("*", cell_start.x + 20, cell_start.y + 10, 50, BLACK);
				}
			} else {
				DrawRectangleRec(cell_start, COL_HIDDEN);
			}
	
			DrawRectangleLinesEx(cell_start, 2, DARKGRAY);
		}
	}
}
