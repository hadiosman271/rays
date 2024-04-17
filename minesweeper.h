#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
	int number;
	bool clear, bomb;
	bool visited; // to avoid infinite recursion when clearing
} Cell;

typedef struct {
	Cell *field;
	int rows, cols, bombs;
	bool initialized;
	bool game_over;
} Minesweeper;

// returns the number of bombs around a cell
int neighbors(Cell field[], int rows, int cols, int x, int y) {
	int n = 0;
	for (int j = -1; j <= 1; j++)
		for (int k = -1; k <= 1; k++) {
			int y1 = y + j, x1 = x + k;
			if (!(j == 0 && k == 0)
				&& x1 >= 0 && x1 < cols && y1 >= 0 && y1 < rows
				&& field[y1 * cols + x1].bomb)
				n++;
		}
	return n;
}

void generate_field(Cell field[], int rows, int cols, int bombs, int first_x, int first_y) {
	srand((unsigned) time(NULL));

	// randomly place bombs
	for (int i = 0; i < bombs; i++) {
		int idx = rand() % (rows * cols);
		while (field[idx].bomb)
			idx = (idx + 1 % (rows * cols));
		Cell c = { .bomb = true };
		field[idx] = c;
	}

	// clear 3x3 area around first cell
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			field[(first_y + i) * cols + first_x + j].bomb = false;

	// set numbers
	for (int i = 0; i < rows * cols; i++) {
		if (!field[i].bomb) {
			Cell c = { .bomb = false, .number = 0 };
			field[i] = c;
			field[i].number = neighbors(field, rows, cols, i % cols, i / cols);
		}
	}
}

// returns true if cell is a bomb
bool clear_cell_recursive(Cell field[], int rows, int cols, int x, int y) {
	int pos = y * cols + x;
	field[pos].clear = true;
	if (field[pos].bomb) {
		return true;
	}

	// only recursively clear empty squares
	if (field[pos].number != 0)
		return false;

	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++) {
			int x1 = x + j, y1 = y + i;

			if ((i == 0 && j == 0)
				|| x1 < 0 || x1 >= cols || y1 < 0 || y1 >= rows)
				continue;

			int p = y1 * cols + x1;
			if (!field[p].visited && !field[p].bomb) {
				field[p].visited = true;
				clear_cell_recursive(field, rows, cols, p % cols, p / cols);
			}
		}

	return false;
}

void clear_cell(Minesweeper *ms, int x, int y) {
	if (ms->game_over) {
		memset(ms->field, 0, sizeof(Cell [ms->rows * ms->cols]));
		ms->initialized = false;
		ms->game_over = false;
	} else {
		if (!ms->initialized) {
			generate_field(ms->field, ms->rows, ms->cols, ms->bombs, x, y);
			ms->initialized = true;
		}
		ms->game_over = clear_cell_recursive(ms->field, ms->rows, ms->cols, x, y);
	}
}

#endif /* minesweeper.h */
