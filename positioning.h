#include "raylib.h"

typedef enum { A_INSIDE, A_RELATIVE } AnchorType;
typedef enum { A_TOP = -1, A_YCENTER, A_BOTTOM } AnchorY;
typedef enum { A_LEFT = -1, A_XCENTER, A_RIGHT } AnchorX;
typedef struct {
	AnchorType type;
	AnchorY y;
	AnchorX x;
} Anchor;

typedef struct Element {
	Vector2 size;
	Vector2 padding;
	Rectangle anchored_to;
	Anchor anchor_pos;
} Element;

Rectangle get_element_bbox(Element e) {
	int x = e.anchored_to.x, y = e.anchored_to.y;
	switch (e.anchor_pos.type) {
	case A_INSIDE:
		switch (e.anchor_pos.x) {
			case A_LEFT:    x += e.padding.x; break;
			case A_XCENTER: x += (e.anchored_to.width - e.size.x) / 2 + e.padding.x; break;
			case A_RIGHT:   x += e.anchored_to.width - e.size.x - e.padding.x; break;
		}
		switch (e.anchor_pos.y) {
			case A_TOP:     y += e.padding.y; break;
			case A_YCENTER: y += (e.anchored_to.height - e.size.y) / 2 + e.padding.y; break;
			case A_BOTTOM:  y += e.anchored_to.height - e.size.y - e.padding.y; break;
		}
		break;
	case A_RELATIVE:
		switch (e.anchor_pos.x) {
			case A_LEFT:    x += -(e.size.x + e.padding.x); break;
			case A_XCENTER: x += (e.anchored_to.width - e.size.x) / 2 + e.padding.x; break;
			case A_RIGHT:   x += e.anchored_to.width + e.padding.x; break;
		}
		switch (e.anchor_pos.y) {
			case A_TOP:     y += -(e.size.y + e.padding.y); break;
			case A_YCENTER: y += (e.anchored_to.height - e.size.y) / 2 + e.padding.y; break;
			case A_BOTTOM:  y += e.anchored_to.height + e.padding.y; break;
		}
		break;
	}

	return (Rectangle) { x, y, e.size.x, e.size.y };
}
