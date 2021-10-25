#include "funcproj.h"
#include "gfx2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define BOARDSZ 700
#define CELLSZ 30

// Print the menu items under the screen while playing. Update the cleared blocks and time count when appropriate
void printMenu(int bombCount, double *lastTime, int *lastClear, time_t start, int clearCount) {
	char bombs[3], timeCount[3], lastTimeCount[3], safe[3], cleared[3], lastCleared[3];
	int y = BOARDSZ + 15;
	time_t now = time(NULL);
	double dif = difftime(now, start);

	sprintf(bombs, "%d", bombCount);
	sprintf(lastTimeCount, "%003.0lf", *lastTime);
	sprintf(timeCount, "%003.0lf", dif);
	sprintf(lastCleared, "%d", *lastClear);
	sprintf(cleared, "%d", clearCount);

	gfx_color(0, 0, 0);
	gfx_text(BOARDSZ/20, y, "BOMBS:");
	gfx_text(BOARDSZ/20 + 50, y, bombs);
	gfx_text(BOARDSZ/2 - 50, y, "TIME:");
	gfx_text(BOARDSZ*4/5, y, "CLEAR:");

	if (clearCount != *lastClear) { // Update/clear the cleared block count
		gfx_color(255, 255, 255);
		gfx_text(BOARDSZ*4/5 + 60, y, lastCleared);
		gfx_color(0, 0, 0);
		gfx_text(BOARDSZ*4/5 + 60, y, cleared);
	}

	if ((int) dif != *lastTime) { // Update/clear the time count
		gfx_color(255, 255, 255);
		gfx_text(BOARDSZ/2 + 20, y, lastTimeCount);
		gfx_color(0, 0, 0);
		gfx_text(BOARDSZ/2 + 20, y, timeCount);
	}
	*lastTime = dif;
	*lastClear = clearCount;
}

// Display the mode options
void modeSelect(void) {
	int len = BOARDSZ/15;
	gfx_text(BOARDSZ/2 - 55, BOARDSZ/3, "SELECT A GAME MODE");
	gfx_rectangle(BOARDSZ/2 - len, BOARDSZ/3 + BOARDSZ/10 - len/2, len*2, len);
	gfx_text(BOARDSZ/2 - 10, BOARDSZ/3 + BOARDSZ/10 + 6, "EASY");
	gfx_rectangle(BOARDSZ/2 - len, BOARDSZ/3 + BOARDSZ/5 - len/2, len*2, len);
	gfx_text(BOARDSZ/2 - 17, BOARDSZ/3 + BOARDSZ/5 + 6, "MEDIUM");
	gfx_rectangle(BOARDSZ/2 - len, BOARDSZ/3 + BOARDSZ*3/10 - len/2, len*2, len);
	gfx_text(BOARDSZ/2 - 10, BOARDSZ/3 + BOARDSZ*3/10 + 6, "HARD");
	gfx_text(BOARDSZ/2 - 38, BOARDSZ/3 + BOARDSZ*4/10 + 6, "INSTRUCTIONS:");
	gfx_text(BOARDSZ/2 - 83, BOARDSZ/3 + BOARDSZ*4/10 + 24, "Left click to reveal a block");
	gfx_text(BOARDSZ/2 - 78, BOARDSZ/3 + BOARDSZ*4/10 + 42, "Right click to place a flag");
}

// Verify a mode option was clicked and return the option
double getMode(void) {
	int x = gfx_xpos(), y = gfx_ypos(), len = BOARDSZ/15;
	if (x >= BOARDSZ/2 - len && x <= BOARDSZ/2 + len) {
		if (y >= BOARDSZ/3 + BOARDSZ/10 - len/2 && y <= BOARDSZ/3 + BOARDSZ/10 + len/2) return 10;
		else if (y >= BOARDSZ/3 + BOARDSZ*2/10 - len/2 && y <= BOARDSZ/3 + BOARDSZ*2/10 + len/2) return 20;
		else if (y >= BOARDSZ/3 + BOARDSZ*3/10 - len/2 && y <= BOARDSZ/3 + BOARDSZ*3/10 + len/2) return 30;
	}
	return 0;
}

// Find the number of bombs surrounding a block
int checkBombs(Block blocks[][CELLSZ], double mode, int srow, int scol) {
	int numBombs = (blocks[srow][scol].type) ? 0 : -1, row, col;

	for (row = -1; row < 2; row++)
		for (col = -1; col < 2; col++)
			if (srow+row >= 0 && scol+col >= 0 && srow+row < mode && scol+col < mode)
				numBombs += (blocks[srow+row][scol+col].type) ? 0 : 1;
	return numBombs;
}

// Initialize the grid of blocks and randomly generate mines
void initGrid(Block blocks[][CELLSZ], double mode, int *bombCount) {
	int row, col, btype;
	double margin = floor((BOARDSZ - 10) / mode), my = floor(margin/5.0), mx = floor(margin/5.0);

	srand(time(0));
	for (row = 0; row < mode; row++) {
		for (col = 0; col < mode; col++) {
			btype = rand() % 2;
			blocks[row][col].type = (btype) ? 1 : 0;
			*bombCount += (btype) ? 0 : 1;
			blocks[row][col].clicked = 0;
			blocks[row][col].xpos = mx + margin*col;
			blocks[row][col].ypos = my + margin*row;
			blocks[row][col].len = floor(margin*4/5.0);
			blocks[row][col].flag = 0;
		}
	}
	gfx_fill_rectangle(mx, BOARDSZ, BOARDSZ - 2*mx, 25);
}

// Display the grid and assign the numBombs struct member
void dispGrid(Block blocks[][CELLSZ], double mode) {
	int row, col;

	for (row = 0; row < mode; row++) {
		for (col = 0; col < mode; col++) {
			blocks[row][col].numBombs = checkBombs(blocks, mode, row, col);
			gfx_color(255, 255, 255);
			gfx_fill_rectangle(blocks[row][col].xpos, blocks[row][col].ypos, blocks[row][col].len, blocks[row][col].len);
		}
	}
}

// Recursive function to activate a block
void clickBox(Block blocks[][CELLSZ], double mode, int row, int col, int *clearCount) {
	double len = blocks[0][0].len / 2;
	int i, j;
	char s[1];

	sprintf(s, "%d", blocks[row][col].numBombs);
	if (blocks[row][col].type) {
		blocks[row][col].clicked = 1;
		(*clearCount)++;
		if (blocks[row][col].numBombs) gfx_color(blocks[row][col].numBombs*35, 0, 255-blocks[row][col].numBombs*35);
		else gfx_color(0, 150, 0);
		gfx_fill_rectangle(blocks[row][col].xpos, blocks[row][col].ypos, blocks[row][col].len, blocks[row][col].len);
		gfx_color(255, 255, 255);
		if (blocks[row][col].numBombs) gfx_text(blocks[row][col].xpos + len - 2 , blocks[row][col].ypos + len + 5, s);

		if (blocks[row][col].numBombs == 0)
			for (i = -1; i < 2; i++)
				for (j = -1; j < 2; j++)
					if (row + i >= 0 && col + j >= 0 && row + i < mode && col + j < mode && blocks[row+i][col+j].clicked == 0)
						clickBox(blocks, mode, row + i, col + j, clearCount);
	}
}

// Check if the click is on a block and not on a margin gap
int checkClick(int x, int y, int margin, int gap) {
	return (x % margin > gap) ? (y % margin > gap) ? 1 : 0 : 0;
}

// Draw a bomb
void drawBomb(Block blocks [][CELLSZ], int len, int x, int y) {
	gfx_color(255, 0, 0);
	gfx_fill_rectangle(x, y, len, len);
	gfx_color(0, 0, 0);
	gfx_fill_circle(x + len/2, y + len/2, len/3);
	gfx_color(255, 255, 255);
	gfx_fill_circle(x + len/3, y + len/3, len/18);
}

// Display game over screen
void gameOver(Block blocks[][CELLSZ], double mode, int *game) {
	int len = BOARDSZ/3, row, col;

	for (row = 0; row < mode; row++)
		for (col = 0; col < mode; col++)
			if (blocks[row][col].type == 0)
				drawBomb(blocks, blocks[row][col].len, blocks[row][col].xpos, blocks[row][col].ypos);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(BOARDSZ/2 - len, BOARDSZ/2 - len/2, len*2, len);
	gfx_color(255, 255, 255);
	gfx_text(BOARDSZ/2 - 25, BOARDSZ/2 - 20, "GAME OVER");
	gfx_text(BOARDSZ/2 - 30, BOARDSZ/2 + 30, "YOU FAILED!");
	gfx_text(BOARDSZ/2 - 67, BOARDSZ/2 + 55, "To play again, press (r)");
	*game = 0;
}

// Find where the click location was and determine what function to call based on the type of the block clicked
void findClick(Block blocks[][CELLSZ], double mode, int x, int y, int *game, int *clearCount, double *time) {
	double margin = floor((BOARDSZ - 10) / mode), gap = floor(margin/5.0), len = blocks[0][0].len/2;
	int row = y/margin, col = x/margin;
	gfx_color(150, 150, 150);
	if (checkClick(x, y, margin, gap)) {
		if (blocks[row][col].type && blocks[row][col].clicked == 0 && blocks[row][col].flag == 0) {
			clickBox(blocks, mode, row, col, clearCount);
		} else if (blocks[row][col].type == 0 && blocks[row][col].flag == 0) {
			gameOver(blocks, mode, game);
		}
	}
}

// Draw a flag
void drawFlag(Block blocks[][CELLSZ], int len, int x, int y) {
	gfx_color(255, 0, 0);
	gfx_fill_rectangle(x + len/5, y + len/5, len*3/5, len*2/5);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(x + len/5, y + len*3/5, len/7, len/5);
}

// Validate that a flag can be placed and determine what function to call
void flag(Block blocks[][CELLSZ], double mode, int x, int y) {
	double margin = floor((BOARDSZ - 10) / mode), gap = floor(margin/5.0), len = blocks[0][0].len ;
	int row = y/margin, col = x/margin;
	gfx_color(150, 150, 150);
	if (checkClick(x, y, margin, gap)) {
		if (blocks[row][col].clicked == 0 && blocks[row][col].flag == 0) {
			drawFlag(blocks, len, blocks[row][col].xpos, blocks[row][col].ypos);
			blocks[row][col].flag = 1;
		} else if (blocks[row][col].clicked == 0 && blocks[row][col].flag == 1) {
			gfx_color(255, 255, 255);
			gfx_fill_rectangle(blocks[row][col].xpos, blocks[row][col].ypos, blocks[row][col].len, blocks[row][col].len);
			blocks[row][col].flag = 0;
		}
	}
}

// Display the win screen and score
void win(double time) {
	char score[3];
	int len = BOARDSZ/3;
	sprintf(score, "%003.0lf", time);
	gfx_color(0, 0, 0);
	gfx_fill_rectangle(BOARDSZ/2 - len, BOARDSZ/2 - len/2, len*2, len);
	gfx_color(255, 255, 255);
	gfx_text(BOARDSZ/2 - 20, BOARDSZ/2 - 20, "YOU WIN");
	gfx_text(BOARDSZ/2 - 35, BOARDSZ/2 + 30, "TIME: ");
	gfx_text(BOARDSZ/2 - 67, BOARDSZ/2 + 55, "To play again, press (r)");
	gfx_text(BOARDSZ/2 + 20, BOARDSZ/2 + 30, score);
}

// Initilize a new game
int initGame(Block blocks[][CELLSZ], double *mode, int *bombCount, int *safeCount, time_t *startTime) {
	char c;

	gfx_clear();
	modeSelect();
	do {
		c = gfx_wait();
		if (c == 1)
			*mode = getMode();
		else if (c == 'q')
			return 0;
	} while(!(*mode));

	gfx_clear();

	initGrid(blocks, *mode, bombCount);
	dispGrid(blocks, *mode);
	*safeCount = (*mode) * (*mode) - (*bombCount);

	*startTime = time(NULL);
	return 1;
}
