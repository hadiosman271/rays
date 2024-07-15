#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>

#define ENEMIES 5
#define PROJECTILES 8

typedef struct {
	Vector2 pos, vel;
	float size;
} Object;

typedef struct {
	Rectangle bbox;
	int line;

	Font font;
	int font_size;
	int padding;
} Logger;

Rectangle screen = { 0, 0, 1920, 1080 };
Rectangle play_area = {};

Vector2 mouse = {};
Vector2 new_mouse = {};
Vector2 last_mouse = {};
float accuracy_thresh = .99;
int target_idx = -1;
float target_dist = 9999;

#define log(logger, text, ...)									\
	(DrawTextEx(												\
		logger.font, TextFormat(text, ##__VA_ARGS__),			\
		(Vector2) {												\
			logger.bbox.x + 5,									\
			logger.bbox.y + 5 + logger.font_size * logger.line	\
		},														\
		logger.font_size, logger.padding, WHITE					\
	), logger.line++)

void draw_arrow(Vector2 start, Vector2 end, float thickness, Color color) {
	Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));

	Vector2 thickness_vec = Vector2Scale(dir, thickness);
	Vector2 arrowhead_start = Vector2Subtract(end, Vector2Scale(dir, thickness * 4));

	DrawLineEx(start, arrowhead_start, thickness, color);
	DrawTriangle(
		end,
		Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec,  3 * PI / 2)),
		Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec, -3 * PI / 2)),
		color
	);
}

void draw_reticle(Vector2 origin, Vector2 target, float thickness, Color color) {
	Vector2 dir = Vector2Normalize(Vector2Subtract(target, origin));

	Vector2 thickness_vec = Vector2Scale(dir, thickness);
	Vector2 arrowhead_start = Vector2Subtract(target, Vector2Scale(dir, thickness));

	DrawLineEx(target, Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec,  2.5 * PI / 2)), 2, color);
	DrawLineEx(target, Vector2Add(arrowhead_start, Vector2Rotate(thickness_vec, -2.5 * PI / 2)), 2, color);

	Vector2 p1 = origin;
	Vector2 p2 = Vector2Add(p1, Vector2Scale(dir, 20));
	float len = Vector2Length(Vector2Subtract(target, origin));
	for (int i = 2; i * 10 < len; i++) {
		if (i % 3 == 0) {
			DrawLineEx(p1, p2, 2, color);
		}
		p1 = p2;
		p2 = Vector2Add(p1, Vector2Scale(dir, 10));
	}
}

void object_move(Object *obj) {
	obj->pos = Vector2Add(obj->pos, obj->vel);
}

void object_move_bounded(Object *obj, Rectangle bounds) {
	obj->pos = Vector2Clamp(
		Vector2Add(obj->pos, obj->vel),
		(Vector2) { bounds.x + obj->size, bounds.y + obj->size },
		(Vector2) { bounds.x + bounds.width - obj->size, bounds.y + bounds.height - obj->size }
	);
}

Vector2 object_target_moving(Object origin, float projectile_speed, Object target) {
	// d = p_2 - p_1, d' = d + v_2
	// dir = norm(|v_1|*d + v_2*|d'|)
	Vector2 d = Vector2Subtract(target.pos, origin.pos);
	float d2_len = Vector2Length(Vector2Add(d, target.vel));
	return Vector2Normalize(
		Vector2Add(
			Vector2Scale(d, projectile_speed),
			Vector2Scale(target.vel, d2_len)
		)
	);
}

Vector2 assist_aim(Object origin, Vector2 mouse, Object target, float accuracy_thresh, Rectangle bounds) {
	Vector2 mouse_dir = Vector2Subtract(mouse, origin.pos);
	float mouse_dist = Vector2Length(mouse_dir);
	Vector2 new_mouse = Vector2Clamp(
		Vector2Add(
			origin.pos,
			Vector2Scale(
				object_target_moving(origin, 50, target),
				mouse_dist
			)
		),
		(Vector2) { bounds.x, bounds.y },
		(Vector2) { bounds.x + bounds.width, bounds.y + bounds.height }
	);

	// TODO: smoothen the transition
	if (Vector2DotProduct(mouse_dir, Vector2Subtract(new_mouse, origin.pos))
		/ (mouse_dist * mouse_dist) < accuracy_thresh) {
		return mouse;
	} else {
		return new_mouse;
	}
}

Vector2 vector2_rand_bounded(int size, Rectangle bounds) {
	return (Vector2) {
		bounds.x + rand() % (int) bounds.width  - size * 2,
		bounds.y + rand() % (int) bounds.height - size * 2
	};
}

void game_input(Object *player, Object enemy[], Object projectile[], int *cooldown) {
	if (IsKeyDown(KEY_W)) {
		player->vel.y = -10;
	}
	if (IsKeyDown(KEY_S)) {
		player->vel.y = 10;
	}
	if (IsKeyDown(KEY_A)) {
		player->vel.x = -10;
	}
	if (IsKeyDown(KEY_D)) {
		player->vel.x = 10;
	}

	mouse = Vector2Clamp(
		GetMousePosition(),
		(Vector2) { play_area.x, play_area.y },
		(Vector2) { play_area.x + play_area.width, play_area.y + play_area.height }
	);

	target_idx = -1;
	target_dist = 999;
	for (int i = 0; i < ENEMIES; i++) {
		float d = Vector2Length(Vector2Subtract(mouse, enemy[i].pos));
		if (d < target_dist) {
			target_dist = d;
			target_idx = i;
		}
	}

	new_mouse = assist_aim(*player, mouse, enemy[target_idx], accuracy_thresh, play_area);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && *cooldown == 0) {
		for (int i = 0; i < PROJECTILES; i++) {
			if (projectile[i].vel.x == 0 && projectile[i].vel.y == 0) {
				projectile[i].pos = player->pos;
				projectile[i].vel = Vector2Scale(
					Vector2Normalize(
						Vector2Subtract(new_mouse, player->pos)
					),
					50
				);
				*cooldown = 5;
				break;
			}
		}
	}
}

void game_update(Object *player, Object enemy[], Object projectile[], int *cooldown) {
	object_move_bounded(player, play_area);
	player->vel = (Vector2) { 0, 0 };

	for (int i = 0; i < ENEMIES; i++) {
		// reflect on walls
		if (enemy[i].pos.x + enemy[i].vel.x < play_area.x + enemy[i].size
			|| enemy[i].pos.x + enemy[i].vel.x > play_area.x - enemy[i].size + play_area.width) {
			enemy[i].vel.x *= -1;
		}
		if (enemy[i].pos.y + enemy[i].vel.y < play_area.y + enemy[i].size
			|| enemy[i].pos.y + enemy[i].vel.y > play_area.y - enemy[i].size + play_area.height) {
			enemy[i].vel.y *= -1;
		}
		object_move_bounded(&enemy[i], play_area);
	}

	for (int i = 0; i < PROJECTILES; i++) {
		if (projectile[i].vel.x != 0 || projectile[i].vel.y != 0) {
			object_move(&projectile[i]);
			if (!CheckCollisionPointRec(projectile[i].pos, play_area)) {
				projectile[i].vel = (Vector2) { 0, 0 };
			}
		}
	}

	*cooldown = *cooldown == 0 ? 0 : *cooldown - 1;
}

void editor_input(Object enemy[], int *held_enemy) {
	mouse = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		for (int i = 0; i < ENEMIES; i++) {
			if (CheckCollisionPointCircle(mouse, enemy[i].pos, enemy[i].size)) {
				*held_enemy = i;
			}
		}
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		*held_enemy = -1;
	}
}

void editor_update(Object enemy[], int *held_enemy) {
	if (*held_enemy != -1) {
		float size = enemy[*held_enemy].size;
		Vector2 clamped_mouse = Vector2Clamp(
			mouse,
			(Vector2) { play_area.x + size, play_area.y + size },
			(Vector2) { play_area.x + play_area.width - size, play_area.y + play_area.height - size }
		);
		enemy[*held_enemy].pos = clamped_mouse;
	}
}

int main(void) {
	InitWindow(screen.width, screen.height, "Aim Assist");
	SetTargetFPS(60);

	play_area = (Rectangle) { screen.x, screen.y, 1280, screen.height };

	Shader shader = LoadShader("res/aim_assist_vert.glsl", "res/aim_assist_frag.glsl");

	Logger logger = {
		.bbox = { play_area.width, 0, screen.width, screen.height },
		.font = LoadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"),
		.font_size = 22, .padding = 5
	};

	Object player = { .size = 10, .pos = { screen.width / 2, screen.height / 2 } };

	srand((unsigned) time(NULL));

	Object enemy[ENEMIES];
	for (int i = 0; i < ENEMIES; i++) {
		enemy[i].size = 20;
		enemy[i].pos = vector2_rand_bounded(enemy[i].size, play_area);
		enemy[i].vel = (Vector2) { 5 * (1 - 2 * (rand() % 2)), 5 * (1 - 2 * (rand() % 2)) };
	}

	Object projectile[PROJECTILES] = {};
	int cooldown = 5;

	bool editing = false;
	int held_enemy = -1;

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_TAB)) {
			last_mouse = mouse;
			editing = !editing;
		}

		if (editing) {
			editor_input(enemy, &held_enemy);
			editor_update(enemy, &held_enemy);
		} else {
			game_input(&player, enemy, projectile, &cooldown);
			game_update(&player, enemy, projectile, &cooldown);
		}


		BeginDrawing();
			ClearBackground(BLACK);

			BeginShaderMode(shader);
				SetShaderValue(shader, GetShaderLocation(shader, "resolution"),
					(float [2]) { screen.width, screen.height }, SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "player_pos"),
					(float [2]) { player.pos.x, player.pos.y }, SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "mouse_pos"),
					editing ?
						(float [2]) { last_mouse.x, last_mouse.y } :
						(float [2]) { mouse.x, mouse.y },
					SHADER_UNIFORM_VEC2
				);
				SetShaderValue(shader, GetShaderLocation(shader, "accuracy_thresh"),
					&accuracy_thresh, SHADER_UNIFORM_FLOAT
				);
				DrawRectangleRec((Rectangle) { 0, 0, screen.width, screen.height }, BLACK);
			EndShaderMode();

			DrawRectangleLinesEx(play_area, 1, DARKGRAY);
			DrawRectangleLinesEx(logger.bbox, 1, DARKGRAY);

			DrawCircleV(player.pos, player.size, RAYWHITE);
			for (int i = 0; i < ENEMIES; i++) {
				DrawCircleV(enemy[i].pos, enemy[i].size, MAROON);
				DrawTextEx(
					logger.font, (const char []) { i + '0', '\0' }, // hacky single digit int to string
					Vector2Subtract(enemy[i].pos, (Vector2) { logger.font_size * .5, logger.font_size * .9 }),
					enemy[i].size * 2, logger.padding, WHITE
				);

				if (editing) {
					if (held_enemy == i) {
						DrawCircleLinesV(enemy[i].pos, enemy[i].size, WHITE);
					}

					draw_arrow(enemy[i].pos, Vector2Add(enemy[i].pos, Vector2Scale(enemy[i].vel, 10)), 5, SKYBLUE);
				}
			}

			draw_reticle(player.pos, new_mouse, 10, DARKGREEN);
			if (!editing) {
				draw_reticle(player.pos, mouse, 10, GRAY);
			}

			for (int i = 0; i < PROJECTILES; i++) {
				if (projectile[i].vel.x != 0 || projectile[i].vel.y != 0) {
					DrawLineEx(projectile[i].pos, Vector2Add(projectile[i].pos, projectile[i].vel), 2, WHITE);
				}
			}

			// logging
			DrawRectangleRec(logger.bbox, BLACK);
			logger.line = 0;
			log(logger, "Enemies:");
			log(logger, "i |  pos             |  vel");
			for (int i = 0; i < ENEMIES; i++) {
				log(logger, "%2d|(%7.2f, %7.2f)|(%6.2f, %6.2f)",
					i,
					enemy[i].pos.x, enemy[i].pos.y,
					enemy[i].vel.x, enemy[i].vel.y
				);
			}
			logger.line++;
			log(logger, "adjusted mouse pos: (%7.2f, %7.2f)", new_mouse.x, new_mouse.y);
			log(logger, "accuracy thresh: %g%%", accuracy_thresh * 100);
			logger.line++;
			log(logger, "targeted enemy: %d", target_idx);
			log(logger, "distance to mouse: %g", target_dist);
			logger.line++;
			log(logger, "mode: %s", editing ? "edit" : "play");
		EndDrawing();
	}

	UnloadShader(shader);
	CloseWindow();
	return 0;
}
