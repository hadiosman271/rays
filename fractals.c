#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

typedef struct {
	const char *name;
	Vector2 pos;
	float scale;
	float radius;
	float min, max;
	float val;
	bool dragging;
} Slider;

typedef struct {
	Rectangle bbox;
	int line;

	Font font;
	int font_size;
	int padding;
} Logger;

#define log(logger, text, ...)									\
	(DrawTextEx(												\
		logger.font, TextFormat(text, ##__VA_ARGS__),			\
		(Vector2) {												\
			logger.bbox.x + 5,									\
			logger.bbox.y + 5 + logger.font_size * logger.line	\
		},														\
		logger.font_size, logger.padding, WHITE					\
	), logger.line++)

void update_slider(Slider *slider) {
	Vector2 mouse = GetMousePosition();
	Vector2 min = slider->pos;
	Vector2 max = { slider->pos.x + (slider->max - slider->min) * slider->scale, slider->pos.y };
	Vector2 pos = { min.x + slider->val * slider->scale, slider->pos.y };
	Rectangle slider_bbox = { min.x, min.y - slider->radius * 2, max.x - min.x, slider->radius * 4 };
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, slider_bbox)) {
		slider->dragging = true;
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		slider->dragging = false;
	}
	if (slider->dragging) {
		slider->val = Vector2Length(Vector2Subtract(Vector2Clamp(mouse, min, max), min)) / slider->scale;
	}
}

void draw_slider(Slider slider, Font font) {
	Vector2 text_pos = (Vector2) { slider.pos.x, slider.pos.y - (font.baseSize + font.glyphPadding * 3) };
	DrawTextEx(font, TextFormat("%s: %d", slider.name, (int) slider.val), text_pos, font.baseSize, font.glyphPadding, WHITE);

	Vector2 min = slider.pos;
	Vector2 max = { slider.pos.x + (slider.max - slider.min) * slider.scale, slider.pos.y };
	Vector2 pos = { min.x + slider.val * slider.scale, slider.pos.y };
	DrawLineEx(min, pos, 10, DARKGRAY);
	DrawLineEx(pos, max, 10, GRAY);
	DrawCircleV(min, slider.radius * .75, DARKGRAY);
	DrawCircleV(max, slider.radius * .75, GRAY);
	DrawCircleV(pos, slider.radius, DARKGRAY);
}

Vector2 vector2_clamp_rect(Vector2 v, Rectangle rec) {
	return Vector2Clamp(v,
		(Vector2) { rec.x, rec.y },
		(Vector2) { rec.x + rec.width, rec.y + rec.height }
	);
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Fractals");
	SetTargetFPS(60);

	//Rectangle win = { screen.x, screen.y, screen.height, screen.height };
	Rectangle win = screen;
	Logger logger = {
		.bbox = { win.height * 1.2, screen.y, screen.width - win.width, screen.height },
		.font = LoadFontEx("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 30, NULL, 0),
		.font_size = 30, .padding = 5
	};
	Slider slider = {
		.name = "max_iters",
		.pos = { logger.bbox.x, (logger.font_size + logger.padding) * 8 },
		.scale = 5, .radius = 10,
		.min = 0, .max = 100, .val = 40,
	};

	Shader shader = LoadShader("res/fractals_vert.glsl", "res/fractals_frag.glsl");
	Vector2 z_0 = { 0., 0. };
	Vector2 c = { -.91, -.25 };
	int plot_c = false;

	Vector2 mouse2 = { 0., 0. };

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_TAB)) {
			plot_c = !plot_c;
		}
		Vector2 middle = { win.width / 2, win.height / 2 };
		Vector2 mouse = GetMousePosition();
		update_slider(&slider);
		int max_iters = slider.val;
		if (!slider.dragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, win)) {
			mouse2 = (Vector2) { mouse.x - middle.x, middle.y - mouse.y };
			if (plot_c) {
				//z_0 = Vector2Scale(mouse2, 2 / screen.width);
			} else {
				c = Vector2Scale(mouse2, 2 / screen.width);
			}
		}

		BeginDrawing();
			ClearBackground(BLACK);
			BeginShaderMode(shader);
				//SetShaderValue(shader, GetShaderLocation(shader, "matProjection"),
				//	MatrixOrtho(), SHADER_UNIFORM_MAT4
				//);
				SetShaderValue(shader, GetShaderLocation(shader, "resolution"),
					//(float [2]) { win.width, win.height }, SHADER_UNIFORM_VEC2
					(float [2]) { 2., 2. * (screen.height / screen.width) }, SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "in_z_0"),
					(float [2]) { z_0.x, z_0.y }, SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "in_c"),
					(float [2]) { c.x, c.y }, SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "plot_c"),
					&plot_c, SHADER_UNIFORM_INT
				);
				SetShaderValue(shader, GetShaderLocation(shader, "max_iters"),
					&max_iters, SHADER_UNIFORM_INT
				);
				DrawRectangleRec((Rectangle) { 0, 0, win.width, win.height }, SKYBLUE);
			EndShaderMode();

			draw_slider(slider, logger.font);
			// logging
			//DrawRectangleRec(logger.bbox, BLACK);
			logger.line = 0;
			log(logger, "z_n = {z_{n-1}}^2 + c");
			log(logger, "press tab to change plot");
			if (plot_c) {
				logger.line += 2;
				log(logger, "plotting c");
				log(logger, "z_0: [%5.2f,%5.2f]", z_0.x, z_0.y);
			} else {
				log(logger, "use mouse to control c");
				logger.line++;
				log(logger, "plotting z_0");
				log(logger, "c: [%5.2f,%5.2f]", c.x, c.y);
			}
		EndDrawing();
	}

	UnloadShader(shader);
	CloseWindow();
	return 0;
}
