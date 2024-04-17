#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>

#define K 100000

#define OBJS 5
#define FIELD_RES 30

typedef struct {
	float charge;
	float radius;
	Vector2 pos, vel, acc;
	Vector2 force;
} Object;

void reset_charges(Object obj[], int objs);
void update_charges(Object obj[], int objs);
void draw_charges(Object obj[], int objs);
void update_field(Vector2 field[], int res, Object obj[], int objs);
void draw_field(Vector2 field[], int res);

void DrawArrow(Vector2 startPos, Vector2 endPos, float thickness, Color color);
float FindWidestNumber(int fontSize);
void DrawVector2Text(const char *name, Vector2 v, int posX, int posY, int fontSize, int spacing, Color color);

Rectangle screen = { 0, 0, 1920, 1080 };

int font_size_small  = 25;
int font_size_normal = 30;
int spacing;

int holding = -1;
int last_held = -1;
Vector2 hold_point;

Vector2 field_start = { 40, 40 };

int main(void) {
	InitWindow(screen.width, screen.height, "Electric Charge");
	SetTargetFPS(60);

	spacing = FindWidestNumber(font_size_small) + 2;

	srand((unsigned) time(NULL));

	Object obj[OBJS] = {
	};
	reset_charges(obj, OBJS);

	Vector2 field[FIELD_RES * FIELD_RES];
	for (int i = 0; i < FIELD_RES * FIELD_RES; i++) {
		field[i] = (Vector2) { 20., 20. };
	}

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_SPACE)) {
			for (int i = 0; i < OBJS; i++) {
				reset_charges(obj, OBJS);
			}
		}

		update_charges(obj, OBJS);
		update_field(field, FIELD_RES, obj, OBJS);

		BeginDrawing();
			ClearBackground(BLACK);

			draw_charges(obj, OBJS);
			draw_field(field, FIELD_RES);

			DrawText("Left Click: Drag and drop object",
				10, 5, font_size_normal, LIGHTGRAY
			);
			DrawText("Right Click: Apply force to selected object",
				10, font_size_normal + 10, font_size_normal, LIGHTGRAY
			);
			DrawText("Space: Reset Objects",
				10, font_size_normal * 2 + 20, font_size_normal, LIGHTGRAY
			);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

void DrawArrow(Vector2 startPos, Vector2 endPos, float thickness, Color color) {
	Vector2 dir = Vector2Normalize(Vector2Subtract(endPos, startPos));

	Vector2 thickness_vec = Vector2Scale(dir, thickness);
	Vector2 arrowhead_start = Vector2Subtract(endPos, Vector2Scale(dir, thickness * 4));

	DrawLineEx(startPos, arrowhead_start, thickness, color);
	DrawTriangle(
		endPos,
		Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec,  3 * PI / 2)),
		Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec, -3 * PI / 2)),
		color
	);
}

float FindWidestNumber(int fontSize) {
	float max = 0;
	for (int i = 0; i < 10; i++) {
		float w = MeasureText((const char []) { i + '0', '\0' }, fontSize);
		if (w > max) max = w;
	}

	return max;
}

void DrawVector2Text(const char *name, Vector2 v, int posX, int posY, int fontSize, int spacing, Color color) {
	const char *start = TextFormat("%s:", name);
	DrawText(start, posX, posY, fontSize, color);
	posX += spacing * 4;

	const char *v_str = TextFormat("%d,%d", (int) v.x, (int) v.y);
	DrawText("[ ", posX, posY, fontSize, color);
	posX += MeasureText("[ ", fontSize);
	for (int i = 0; v_str[i] != '\0'; i++) {
		DrawText((const char []) { v_str[i], '\0' }, posX, posY, fontSize, color);
		posX += spacing;
	}
	DrawText("]", posX, posY, fontSize, color);
}

void reset_charges(Object obj[], int objs) {
	for (int i = 0; i < objs; i++) {
		obj[i] = (Object) {};
		obj[i].charge = rand() % 20 + 1;
		if (obj[i].charge > 10) {
			obj[i].charge = -obj[i].charge + 10;
		}
		obj[i].radius = fabs(obj[i].charge) * 15;
		obj[i].pos.x = (rand() % (int) (screen.width  - obj[i].radius * 2)) + obj[i].radius;
		obj[i].pos.y = (rand() % (int) (screen.height - obj[i].radius * 2)) + obj[i].radius;
	}
}

void update_charges(Object obj[], int objs) {
		Vector2 extra_force = {};
		if (last_held != -1 && holding == -1) {
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
				extra_force = Vector2Scale(Vector2Normalize(Vector2Subtract(GetMousePosition(), obj[last_held].pos)), 500);
			}
		}

		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			for (int i = objs - 1; i >= 0; i--) {
				if (CheckCollisionPointCircle(mouse, obj[i].pos, obj[i].radius)) {
					holding = i;
					last_held = i;
					hold_point = Vector2Subtract(mouse, obj[i].pos);
				}
			}
			if (holding == -1) last_held = -1;
		}
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			holding = -1;
		}

		if (holding != -1) {
			obj[holding].pos = Vector2Clamp(
				Vector2Subtract(mouse, hold_point),
				(Vector2) { 0, 0 }, (Vector2) { screen.width, screen.height }
			);
		}


		for (int i = 0; i < objs; i++) {
			Vector2 f = {};
			for (int j = 0; j < objs; j++) {
				if (i == j) continue;
				// f = q1 * q2 / length(p1 - p2)**2 * norm(p1 - p2)
				Vector2 diff = Vector2Subtract(obj[i].pos, obj[j].pos);
				f = Vector2Add(f, Vector2Scale(
					Vector2Normalize(diff),
					Clamp(K * obj[i].charge * obj[j].charge / Vector2LengthSqr(diff), -500, 500)
				));
			}
			obj[i].force = f;
		}

		for (int i = 0; i < objs; i++) {
			if (i == last_held) {
				obj[i].force = Vector2Add(obj[i].force, extra_force);
			}

			obj[i].acc = Vector2Scale(obj[i].force, 1 / fabs(obj[i].charge));

			float dt = GetFrameTime();
			obj[i].pos = Vector2Clamp(
				Vector2Add(obj[i].pos, Vector2Scale(obj[i].vel, dt)),
				(Vector2) { 0, 0 }, (Vector2) { screen.width, screen.height }
			);
			obj[i].vel = Vector2Add(obj[i].vel, Vector2Scale(obj[i].acc, dt));
		}
}

void draw_charges(Object obj[], int objs) {
	for (int i = 0; i < objs; i++) {
		Color color = obj[i].charge < 0 ? ORANGE : SKYBLUE;
		if (last_held == i) {
			DrawCircleV(obj[i].pos, obj[i].radius, RAYWHITE);
			DrawCircleV(obj[i].pos, obj[i].radius * .95, color);	
		} else {
			DrawCircleV(obj[i].pos, obj[i].radius, color);
		}

		// draw stats
		DrawText(TextFormat("charge: %d", (int) obj[i].charge),
			obj[i].pos.x + obj[i].radius + 10, obj[i].pos.y - obj[i].radius,
			font_size_small, LIGHTGRAY
		);
		DrawVector2Text("pos", obj[i].pos,
			obj[i].pos.x + obj[i].radius + 10, obj[i].pos.y - obj[i].radius + font_size_small + 5,
			font_size_small, spacing, LIGHTGRAY
		);
		DrawVector2Text("vel", obj[i].vel,
			obj[i].pos.x + obj[i].radius + 10, obj[i].pos.y - obj[i].radius + font_size_small * 2 + 5,
			font_size_small, spacing, LIGHTGRAY
		);
		DrawVector2Text("acc", obj[i].acc,
			obj[i].pos.x + obj[i].radius + 10, obj[i].pos.y - obj[i].radius + font_size_small * 3 + 5,
			font_size_small, spacing, LIGHTGRAY
		);

		// velocity arrow: pos -> pos + vel
		DrawArrow(
			obj[i].pos,
			Vector2Add(obj[i].pos, obj[i].vel),
			Clamp(Vector2Length(obj[i].vel) / 5, .1, 10),
			RED
		);
		// acceleration arrow: pos + vel -> pos + vel + acc
		DrawArrow(
			Vector2Add(obj[i].pos, obj[i].vel),
			Vector2Add(Vector2Add(obj[i].pos, obj[i].vel), obj[i].acc),
			Clamp(Vector2Length(obj[i].acc) / 5, .1, 10),
			YELLOW
		);
		// force arrow: pos - radius * norm(f) - f -> pos - radius * norm(f)
		Vector2 f_end = Vector2Subtract(
			obj[i].pos,
			Vector2Scale(Vector2Normalize(obj[i].force), obj[i].radius)
		);
		DrawArrow(
			Vector2Subtract(f_end, obj[i].force),
			f_end,
			Clamp(Vector2Length(obj[i].force) / 5, .1, 10),
			PURPLE
		);

		// object index
		DrawText((const char []) { i + '0', '\0' },
			obj[i].pos.x - font_size_normal / 2.5, obj[i].pos.y - font_size_normal / 2.5,
			font_size_normal, RAYWHITE
		);
	}
}

void update_field(Vector2 field[], int x, Object obj[], int objs) {
	int width = screen.width / x;

	for (int i = 0; i < x * x; i++) {
		Vector2 pos = { field_start.x + width * (i % x), field_start.y + width * (int) (i / x) };
		Vector2 e = {};
		for (int j = 0; j < objs; j++) {
			// e = q / length(p1 - p2)**2 * norm(p1 - p2)
			Vector2 diff = Vector2Subtract(pos, obj[j].pos);
			e = Vector2Add(e, Vector2Scale(
				Vector2Normalize(diff),
				Clamp(K * obj[j].charge / Vector2LengthSqr(diff), -50, 50)
			));
		}
		field[i] = e;
	}
}

void draw_field(Vector2 field[], int x) {
	int width = screen.width / x;
	for (int i = 0; i < x * x; i++) {
		Vector2 p1 = { field_start.x + width * (i % x), field_start.y + width * (int) (i / x) };
		DrawArrow(p1, Vector2Add(p1, field[i]), Clamp(Vector2Length(field[i]) / 10, .1, 10), RAYWHITE);
	}
}
