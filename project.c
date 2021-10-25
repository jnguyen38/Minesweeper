/* CSE 20311
 * Jonathan Nguyen
 * project.c
 * My project is to replicate the game of Minesweeper
 */

#include "funcproj.h"
#include "gfx2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define BOARDSZ 700
#define GRIDSZ 30

int main() {
	char c;
	double mode, lastTime = -1;
	int bombCount = 0, game = 1, clearCount = 0, safeCount = 0, lastClear = -1;
	time_t startTime;
	Block blocks[GRIDSZ][GRIDSZ];

	gfx_open(BOARDSZ, BOARDSZ + 25, "MAIN");
	
	c = initGame(blocks, &mode, &bombCount, &safeCount, &startTime);
	if (!c) return 0;

	while(1) {
		if (game) printMenu(bombCount, &lastTime, &lastClear, startTime, clearCount);
		if (gfx_event_waiting()) {
			c = gfx_wait();
			switch (c) {
				case 'q': // Quit
					return 0;
				case 1: // Reveal a block
					if (game)	findClick(blocks, mode, gfx_xpos(), gfx_ypos(), &game, &clearCount, &lastTime);
					if (clearCount == safeCount) {
						game = 0;
						win(lastTime);
					}
					break;
				case 3: // Place a flag
					if (game)	flag(blocks, mode, gfx_xpos(), gfx_ypos());
					break;
				case 'r': // Replay if the game has ended
					if (!game) {
						bombCount = 0;
						safeCount = 0;
						clearCount = 0;
						game = 1;
						c = initGame(blocks, &mode, &bombCount, &safeCount, &startTime);
						if (!c) return 0;
					}
			}
		}
	}
	return 0;
}
