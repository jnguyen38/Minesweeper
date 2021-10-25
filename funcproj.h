/* CSE 20311
 * Jonathan Nguyen
 * funcproj.h
 */

#define GRIDSZ 30
#include <time.h>

// Struct for the minesweeper blocks
typedef struct {
	int type; // type = 1 is a safe block and type = 0 is a bomb
	int numBombs;
	int clicked;
	int xpos;
	int ypos;
	int len;
	int flag;
} Block;

// Function prototypes
void printMenu(int, double *, int *, time_t, int);
void modeSelect(void);
double getMode(void);
int checkBombs(Block [][GRIDSZ], double, int, int);
void initGrid(Block [][GRIDSZ], double, int *);
void dispGrid(Block [][GRIDSZ], double);
void gameOver(Block [][GRIDSZ], double, int *);
void clickBox(Block [][GRIDSZ], double, int, int, int *);
int checkClick(int, int, int, int);
void drawBomb(Block [][GRIDSZ], int, int, int);
void gameOver(Block [][GRIDSZ], double, int *);
void findClick(Block [][GRIDSZ], double, int, int, int *, int *, double *);
void drawFlag(Block [][GRIDSZ], int, int, int);
void flag(Block [][GRIDSZ], double, int, int);
void win(double);
int initGame(Block [][GRIDSZ], double *, int *, int *, time_t *);


