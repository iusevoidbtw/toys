#include <syscall.h>

/* macros */
#define PRINT(s) syscall(SYS_write, 1, s, sizeof(s) - 1);

#define PLAYER 'X'
#define BOT 'O'
#define DRAW 'D'
#define EMPTY ' '

/* function declarations */
static int botmove(void);
static void displayboard(void);
static char status(void);

/* global variables */
static char board[10];
static const int OPTIMAL_MOVES[] = {4, 0, 2, 6, 8, 1, 3, 5, 7};
static const int WINNING_ROWS[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, \
				{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, \
				{0, 4, 8}, {2, 4, 6}};

/* function implementations */
static int
botmove(void)
{
	char tmp;
	int foundmove = 0;
	int move = 0;

	/* if possible to win by playing some move, play that move */
	while (!foundmove && move < 10) {
		if (board[move] == EMPTY) {
			board[move] = BOT;
			if (status() == BOT) {
				foundmove = 1;
			}
			board[move] = EMPTY;
		}
	
		if (foundmove == 0) {
			++move;
		}
	}

	/* otherwise just play some random optimal move */
	if (!foundmove) {
		int cell;
		move = 0;

		for (cell = 0; cell < 9; ++cell) {
			if (board[OPTIMAL_MOVES[cell]] == EMPTY) {
				move = OPTIMAL_MOVES[cell];
				foundmove = 1;
				break;
			}
		} }

	PRINT("taking square ");
	tmp = move + '0';
	syscall(SYS_write, 1, &tmp, 1);
	PRINT(". \n");
	return move;
}

static void
displayboard(void)
{
	int i = 0, j = 0;

	while (i < 9) {
		PRINT("\t ");
		for (j = 0; j < 3; ++i, ++j) {
			syscall(SYS_write, 1, board + i, 1);
			if (j < 2) PRINT(" | ");
		}
		PRINT("\n");
		if (i < 8) PRINT("\t-----------\n");
		j = 0;
	}
}

static char
status(void)
{
	int row, cell;
	int isdraw = 1;
	
	for (row = 0; row < 8; ++row) {
		if (board[WINNING_ROWS[row][0]] != EMPTY && \
			board[WINNING_ROWS[row][0]] == board[WINNING_ROWS[row][1]] && \
			board[WINNING_ROWS[row][1]] == board[WINNING_ROWS[row][2]]) {
			return board[WINNING_ROWS[row][0]];
		}
	}

	for (cell = 0; cell < 9; ++cell) {
		if (board[cell] == EMPTY) {
			isdraw = 0;
		}
	}

	if (isdraw == 1) {
		return DRAW;
	}

	return EMPTY;
}

int
main(void)
{
	char winner = EMPTY;
	char tmp[2];
	int playerturn = 1;
	int move, cell;
	for (cell = 0; cell < 9; ++cell) {
		board[cell] = EMPTY;
	}
	board[9] = '\0';

	PRINT("welcome to tic-tac-toe. make your move by inputting the square's number (1-9).\n");
	displayboard();

	while (winner == EMPTY) {
		if (playerturn == 1) {
			for (;;) {
				do {
					PRINT("enter your move (1-9): ");
					syscall(SYS_read, 0, tmp, 2);
					move = (*tmp - '0') - 1;
				} while (move < 0 || move > 8);
				if (board[move] == EMPTY) {
					break;
				}
				PRINT("that square is already taken.\n");
			}
			board[move] = PLAYER;
		} else {
			move = botmove();
			board[move] = BOT;
		}

		winner = status();
		displayboard();
		playerturn = !playerturn;
	}

	if (winner == DRAW) {
		PRINT("\nit's a draw.\n");
	} else if (winner == PLAYER) {
		PRINT("\nyou won!\n");
	} else if (winner == BOT) {
		PRINT("\nyou lost.\n");
	}

	return 0;
}
