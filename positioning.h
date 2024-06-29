#include "raylib.h"

typedef enum { A_INSIDE, A_RELATIVE } AnchorType;
typedef enum { A_TOP = -1, A_YCENTER, A_BOTTOM } AnchorY;
typedef enum { A_LEFT = -1, A_XCENTER, A_RIGHT } AnchorX;
typedef struct {
	AnchorType type;
	AnchorY y;
	AnchorX x;
} Anchor;

Rectangle get_bbox(
	Vector2 size,
	Vector2 padding,
	Rectangle anchored_to,
	Anchor anchor_pos
) {
	float x = anchored_to.x, y = anchored_to.y;
	switch (anchor_pos.type) {
	case A_INSIDE:
		switch (anchor_pos.x) {
			case A_LEFT:    x += padding.x; break;
			case A_XCENTER: x += (anchored_to.width - size.x) / 2 + padding.x; break;
			case A_RIGHT:   x += anchored_to.width - size.x - padding.x; break;
		}
		switch (anchor_pos.y) {
			case A_TOP:     y += padding.y; break;
			case A_YCENTER: y += (anchored_to.height - size.y) / 2 + padding.y; break;
			case A_BOTTOM:  y += anchored_to.height - size.y - padding.y; break;
		}
		break;
	case A_RELATIVE:
		switch (anchor_pos.x) {
			case A_LEFT:    x += -(size.x + padding.x); break;
			case A_XCENTER: x += (anchored_to.width - size.x) / 2 + padding.x; break;
			case A_RIGHT:   x += anchored_to.width + padding.x; break;
		}
		switch (anchor_pos.y) {
			case A_TOP:     y += -(size.y + padding.y); break;
			case A_YCENTER: y += (anchored_to.height - size.y) / 2 + padding.y; break;
			case A_BOTTOM:  y += anchored_to.height + padding.y; break;
		}
		break;
	}

	return (Rectangle) { x, y, size.x, size.y };
}
