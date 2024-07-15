#include "raylib.h"
#include "raymath.h"

#define EXTRA_IMPL
#include "extra.h"

#define MAX_ELEMENTS 10
#define LINE_THICKNESS 10
#define GRID_SIZE LINE_THICKNESS
#define RESIZE_TOLERANCE 20
#define DOUBLE_CLICK_MAX_FRAMES 15

typedef enum { MOVE, RESIZE } Mode;

typedef struct {
	int elements;
	Rectangle element[MAX_ELEMENTS];

	int hovered;
	int holding;
	Vector2 hold_point;
	int selected;

	ClickTimer click_timer;

	Mode mode;
	bool show_text;
} State;

Rectangle screen = { 0, 0, 1920, 1080 };

void update_mode_move(State *s) {
	Vector2 mouse = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		s->holding = s->hovered;
		s->hold_point.x = mouse.x - s->element[s->holding].x;
		s->hold_point.y = mouse.y - s->element[s->holding].y;
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		s->selected = s->holding;
		s->holding = -1;
	}
	if (s->holding != -1) {
		s->element[s->holding].x = fround(mouse.x - s->hold_point.x, GRID_SIZE);
		s->element[s->holding].y = fround(mouse.y - s->hold_point.y, GRID_SIZE);
	}
}

void update_mode_resize(State *s) {
	Vector2 mouse = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		for (int i = 0; i < s->elements; i++) {
			Vector2 corner = { s->element[i].x + s->element[i].width, s->element[i].y + s->element[i].height };
			if (Vector2Length(Vector2Subtract(mouse, corner)) < RESIZE_TOLERANCE) {
				s->holding = i;
			}
		}
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		s->selected = s->holding;
		s->holding = -1;
	}
	if (s->holding != -1) {
		s->element[s->holding].width  = fmax(LINE_THICKNESS, fround(mouse.x - s->element[s->holding].x, GRID_SIZE));
		s->element[s->holding].height = fmax(LINE_THICKNESS, fround(mouse.y - s->element[s->holding].y, GRID_SIZE));
	}
}

// TODO: use an icon
void draw_cursor_move(void) {
	float thickness = 2;
	float len = 15;
	Vector2 mouse = GetMousePosition();
	DrawLineEx(Vector2Add(mouse, (Vector2) { -len, 0 }), Vector2Add(mouse, (Vector2) { len, 0 }), thickness, WHITE);
	DrawLineEx(Vector2Add(mouse, (Vector2) { 0, -len }), Vector2Add(mouse, (Vector2) { 0, len }), thickness, WHITE);
}

// TODO: use an icon
void draw_cursor_resize(void) {	
	float thickness = 2;
	float len = 15 / M_SQRT2;
	Vector2 mouse = GetMousePosition();
	DrawLineEx(Vector2Add(mouse, (Vector2) { -len, -len }), Vector2Add(mouse, (Vector2) {  len, len }), thickness, WHITE);
	DrawLineEx(Vector2Add(mouse, (Vector2) {  len, -len }), Vector2Add(mouse, (Vector2) { -len, len }), thickness, WHITE);
}

int main(void) {
	InitWindow(screen.width, screen.height, "Layout Editor");
	SetTargetFPS(60);

	Font info_font = LoadFontEx("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 20., NULL, 0);

	State s = {
		.elements = 1,
		.element[0] = { 500, 500, 150, 150 },
		.holding = -1, .selected = -1,
		.hovered = -1, 
		.mode = MOVE, .show_text = true
	};

	while (!WindowShouldClose()) {
		Vector2 mouse = GetMousePosition();

		// find hovered over rect
		s.hovered = -1;
		for (int i = 0; i < s.elements; i++) {
			if (CheckCollisionPointRec(mouse, s.element[i])) {
				// doesn't overwrite if one has already been found
				if (s.hovered == -1) s.hovered = i;

				// only overwrite if smaller
				// without this, unexpected rects get chosen which is annoying
				if (s.element[i].width < s.element[s.hovered].width
					&& s.element[i].height < s.element[s.hovered].height
				) {
					s.hovered = i;
				}
			}
		}

		// mode switching
		if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			s.mode = MOVE;
			for (int i = 0; i < s.elements; i++) {
				Vector2 corner = { s.element[i].x + s.element[i].width, s.element[i].y + s.element[i].height };
				if (Vector2Length(Vector2Subtract(mouse, corner)) < RESIZE_TOLERANCE) {
					s.mode = RESIZE;
				}
			}
		}
		if (IsKeyPressed(KEY_TAB)) {
			s.show_text = !s.show_text;
		}

		update_click_timer(&s.click_timer, DOUBLE_CLICK_MAX_FRAMES);

		// update based on mode
		switch (s.mode) {
		case MOVE:
			update_mode_move(&s);
			break;
		case RESIZE:
			update_mode_resize(&s);
			break;
		}

		// add new square
		if (s.selected == -1 && s.elements < MAX_ELEMENTS - 1
			&& is_double_clicked(s.click_timer, MOUSE_BUTTON_LEFT)
		) {
			s.element[s.elements] = (Rectangle) { mouse.x - 50, mouse.y - 50, 150, 150 };
			s.elements++;
		}
		// delete selected element
		if (s.selected != -1 && s.elements > 0
			&& is_double_clicked(s.click_timer, MOUSE_BUTTON_RIGHT)
		) {
			s.element[s.selected] = s.element[s.elements - 1];
			s.elements--;
			s.selected = -1;
		}

		BeginDrawing();	
			ClearBackground(BLACK);
			for (int i = 0; i < s.elements; i++) {
				Color color = s.selected == i || s.holding == i ? WHITE : LIGHTGRAY;
				DrawRectangleLinesEx(s.element[i], LINE_THICKNESS, color);
				if (s.show_text) {
					Logger rect_info = {
						.bbox = s.element[i], .padding = 15.,
						.font = info_font, .font_size = info_font.baseSize
					};
					log_draw(rect_info, "x: %g", s.element[i].x);
					log_draw(rect_info, "y: %g", s.element[i].y);
					log_draw(rect_info, "width: %g", s.element[i].width);
					log_draw(rect_info, "height: %g", s.element[i].height);
				}
			}

			switch (s.mode) {
			case MOVE:
				draw_cursor_move();
				break;
			case RESIZE:
				draw_cursor_resize();
				break;
			}
		EndDrawing();
	}

	UnloadFont(info_font);

	CloseWindow();
	return 0;
}
