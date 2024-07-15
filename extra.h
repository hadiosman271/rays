#ifndef EXTRA_H
#define EXTRA_H

#include "raylib.h"
#include "raymath.h"

#include <stdio.h>  // for: log()
#include <stdlib.h> // for: vector2_rand_bounded()
#include <time.h>   // for: vector2_rand_bounded()


typedef struct {
	Rectangle bbox;
	int padding;
	int line;

	Font font;
	int font_size;
	int font_padding;
} Logger;

#define log_draw(logger, text, ...) (					            	     \
	DrawTextEx(					    						            	 \
		logger.font, TextFormat(text, ##__VA_ARGS__),		            	 \
		(Vector2) {											            	 \
			logger.bbox.x + logger.padding,									 \
			logger.bbox.y + logger.padding + logger.font_size * logger.line	 \
		},													            	 \
		logger.font_size, logger.font_padding, WHITE				         \
	), logger.line++                                                         \
)

#define LOG_BUFSIZE 512
extern char log_buf[LOG_BUFSIZE];
extern int log_buf_len;
extern int _log_ret;

#define log(str, ...) (                                                                     \
	_log_ret = snprintf(log_buf + log_buf_len, LOG_BUFSIZE - log_buf_len, str, ##__VA_ARGS__),  \
	log_buf_len += _log_ret < 0 ? 0 : _log_ret,                                                 \
	log_buf_len >= LOG_BUFSIZE ? log_buf_len = LOG_BUFSIZE : 0                                  \
)


void draw_log_win(Logger logger);
void draw_arrow(Vector2 start, Vector2 end, float thickness, Color color);


Vector2 vector2_clamp_rec(Vector2 v, Rectangle rec);
Vector2 vector2_rand_bounded(float size, Rectangle bounds);

float fround(float x, float w);


typedef struct {
	int left;
	int right;
} ClickTimer;

void update_click_timer(ClickTimer *t, int double_click_frames);
bool is_double_clicked(ClickTimer t, MouseButton button);


// IMPLEMENTATION
#if defined(EXTRA_IMPL)


char log_buf[LOG_BUFSIZE];
int log_buf_len = 0;
int _log_ret;

void draw_log_win(Logger logger) {
	int logger_width = (logger.bbox.width - logger.padding) / (logger.font_size / 2 + logger.font_padding);

	int start = 0;
	int end = 0;
	while (start < log_buf_len) {
		// go to end, bbox width, or newline, whichever is first
		while (end < log_buf_len && end < start + logger_width && log_buf[end] != '\n') {
			end++;
		}

		char last = log_buf[end];
		log_buf[end] = '\0'; // replace with null for printing
		log_draw(logger, "%s", log_buf + start);
		log_buf[end] = last; // give it back

		start = end + 1, end++;
	}
}

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

Vector2 vector2_clamp_rec(Vector2 v, Rectangle rec) {
	return Vector2Clamp(v,
		(Vector2) { rec.x, rec.y },
		(Vector2) { rec.x + rec.width, rec.y + rec.height }
	);
}

Vector2 vector2_rand_bounded(float size, Rectangle bounds) {
	return (Vector2) {
		bounds.x + size + rand() % (int) (bounds.width  - size * 2),
		bounds.y + size + rand() % (int) (bounds.height - size * 2)
	};
}

// rounds x to the nearest integer multiple of w
float fround(float x, float w) {
	int lower = (int) (x / w) * w;
	return x - lower < x - lower + w ? lower : lower + w;
}

void update_click_timer(ClickTimer *t, int double_click_frames) {
	if (t->left > 0) {
		t->left--;
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (t->left == 0) {
			t->left = double_click_frames;
		} else {
			t->left = 0;
		}
	}
	if (t->right > 0) {
		t->right--;
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		if (t->right == 0) {
			t->right = double_click_frames;
		} else {
			t->right = 0;
		}
	}
}

// double click if timer active when clicked
// timer activates after a click
// timer cancels if active when clicked
//   click     check     update
// 1:  +.........F.........|______
// 2:   _________F_________|______
// 3:  +_________T_________|......
// for whatever fucking reason it only works when you check if the timer is inactive
// its like the order of the check and the update doesnt matter, wtf
// i wasted 2 hours of my life on this
bool is_double_clicked(ClickTimer t, MouseButton button) {
	if (IsMouseButtonPressed(button)) {
		switch (button) {
		case MOUSE_BUTTON_LEFT:
			return t.left == 0;
		case MOUSE_BUTTON_RIGHT:	
			return t.right == 0;
		default:
			return false;
		}
	} else {
		return false;
	}
}

#endif // EXTRA_IMPL

#endif // EXTRA_H
