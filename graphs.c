#include "raylib.h"

#define EXTRA_IMPL
#include "extra.h"

#define MAX_NODES 10
#define MAX_EDGES ((MAX_NODES * (MAX_NODES - 1)) / 2)

typedef struct {
	int nodes;
	Vector2 node[MAX_NODES];
	bool edge[MAX_EDGES];
} Graph;

int edge_idx(int n1, int n2) {
	if (n1 == n2) return -1;
	if (n1 > n2) { // swap
		int tmp = n2; n2 = n1; n1 = tmp;
	}
	return ((n1 * (n1 - 1)) / 2) + n2;
}

void randomize_graph(Graph *graph, float radius, Rectangle bounds) {
	for (int i = 0; i < graph->nodes; i++) {
		float scale = .8;
		Rectangle scaled_bounds = { 
			bounds.width * (1 - scale) / 2, bounds.height * (1 - scale) / 2,
			bounds.width * scale, bounds.height * scale
		};
		graph->node[i] = vector2_rand_bounded(radius, scaled_bounds);
		if (i > 0) {
			graph->edge[edge_idx(i, i - (rand() % i))] = !(rand() % graph->nodes / 3);
		}
	}
}

int main(void) {
	Rectangle screen = { 0, 0, 1920, 1080 };
	InitWindow(screen.width, screen.height, "Graphs");
	SetTargetFPS(60);

	srand((unsigned) time(NULL));

	Rectangle win = { 10., 10., 1290., 1060. };

	Font log_font = LoadFontEx("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 30., NULL, 0);
	Logger logger = {
		.bbox = { 1310, 10, 600, 1060 }, .padding = 5.,
		.font = log_font, .font_size = log_font.baseSize
	};

	Graph graph = { .nodes = 8 };
	log("nodes: %d", graph.nodes);

	float node_radius = 20.;
	randomize_graph(&graph, node_radius, win);

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_SPACE)) {
			randomize_graph(&graph, node_radius, win);
		}

		BeginDrawing();
			ClearBackground(BLACK);
			for (int i = 0; i < graph.nodes; i++) {
				for (int j = 0; j < graph.nodes; j++) {
					if (i != j && graph.edge[edge_idx(i, j)]) {
						DrawLineEx(graph.node[i], graph.node[j], node_radius * .3, GRAY);
					}
				}
			}
			for (int i = 0; i < graph.nodes; i++) {
				DrawCircleV(graph.node[i], node_radius, LIGHTGRAY);
				DrawText(
					TextFormat("%d", i),
					graph.node[i].x - node_radius * (i == 1 ? .3 : .5),
					graph.node[i].y - node_radius * .81,
					node_radius * 2., BLACK
				);
			}
			DrawRectangleLinesEx(win, 1., DARKGRAY);
	
			// log window
			DrawRectangleLinesEx(logger.bbox, 1., DARKGRAY);
			draw_log_win(logger);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
