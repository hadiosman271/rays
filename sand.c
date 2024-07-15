#include "raylib.h"
#include "raymath.h"

#include <stdlib.h>
#include <time.h>

#define SAND_COLORS 5
#define SAND_PALLETE (Color [SAND_COLORS]) { \
    (Color) { 235, 204,   52, 255 },         \
    (Color) { 212, 182,   36, 255 },         \
    (Color) { 232, 198,   35, 255 },         \
    (Color) { 242, 209,   51, 255 },         \
    (Color) { 209, 179,   38, 255 },         \
}

#define TILE_TYPES 3
typedef enum { AIR, SAND, WATER } TileType;
typedef struct {
    TileType type;
    int color_idx;
} Tile;

#define GRID_SIZE 60
Tile grid[GRID_SIZE][GRID_SIZE];

const char *name_from_tile(TileType c) {
    switch (c) {
        case AIR:   return "air";
        case SAND:  return "sand";
        case WATER: return "water";
    }
}

void update_grid(Tile grid[GRID_SIZE][GRID_SIZE]) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = GRID_SIZE - 1; y >= 0; y--) {
            switch (grid[x][y].type) {
            case AIR:
                break;
            case SAND:
                if (y + 1 < GRID_SIZE) {
                    // fall one tile
                    if (grid[x][y + 1].type == AIR) {
                        grid[x][y].type = AIR;
                        grid[x][y + 1].type = SAND;
                        grid[x][y + 1].color_idx = rand() % SAND_COLORS; // randomize color
                    } else if (grid[x][y + 1].type == SAND) {
                        if (x - 1 >= 0 && grid[x - 1][y +  1].type == AIR) {
                            // fall left first
                            grid[x][y].type = AIR;
                            grid[x - 1][y + 1].type = SAND;
                            grid[x - 1][y + 1].color_idx = rand() % SAND_COLORS;
                        } else if (x + 1 < GRID_SIZE - 1 && grid[x + 1][y + 1].type == AIR) {
                            // fall right if left is taken
                            grid[x][y].type = AIR;
                            grid[x + 1][y + 1].type = SAND;
                            grid[x + 1][y + 1].color_idx = rand() % SAND_COLORS;
                        }
                    }
                }
                break;
                case WATER:
                    break;
            }
        }
    }
}

void draw_grid(Tile grid[GRID_SIZE][GRID_SIZE], float cell_width) {
    Vector2 start = { 2., 5. };
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            Color color = BLACK;
            switch (grid[i][j].type) {
            case AIR:
                color = BLACK;
                break;
            case SAND:
                color = SAND_PALLETE[grid[i][j].color_idx];
                break;
            case WATER:
                color = BLUE;
                break;
            }
            DrawRectangle(i * cell_width + start.x, j * cell_width + start.y, cell_width, cell_width, color);
            DrawRectangleLines(start.x, start.y, (i + 1) * cell_width, (j + 1) * cell_width, DARKGRAY);
        }
    }
}

int main(void) {
    Rectangle screen = { 0, 0, 1920, 1080 };
    InitWindow(screen.width, screen.height, "Sand");
    SetTargetFPS(60);

    srand((unsigned) time(NULL));

    float cell_width = screen.height / (GRID_SIZE + 1);
    TileType placing = SAND;

    int idx_x = 0, idx_y = 0;

    while (!WindowShouldClose()) {
        Vector2 mouse = Vector2Clamp(
            Vector2Scale(GetMousePosition(), 1 / cell_width),
            (Vector2) { 0, 0 }, (Vector2) { GRID_SIZE - 1, GRID_SIZE - 1 }
        );
        idx_x = mouse.x, idx_y = mouse.y;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            grid[idx_x][idx_y].type = placing;
            if (placing == SAND) {
                grid[idx_x][idx_y].color_idx = rand() % SAND_COLORS;
            }
        }

        if (IsKeyPressed(KEY_TAB)) {
            placing = (placing + 1) % TILE_TYPES;
        }
        
        update_grid(grid);

        BeginDrawing();
            ClearBackground(BLACK);

            draw_grid(grid, cell_width);

            // logging
            int line = 0;
            float font_size = 30.;
            float padding = 5.;
            Vector2 log_win_start = { GRID_SIZE * cell_width + padding };
        
            DrawText(TextFormat("Placing %s at [%d, %d]", name_from_tile(placing), idx_x, idx_y),
                log_win_start.x, log_win_start.y + line * (font_size + padding),
                font_size, WHITE
            );
            line++;

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
