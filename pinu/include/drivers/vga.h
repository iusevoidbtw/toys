/* See UNLICENSE file for copyright and license details. */

#ifndef _VGA_H
#define _VGA_H

#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

#define VGA_BACKSPACE 0x08

/* screen io ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

void clear_screen(void);
int get_col(int);
int get_cursor_pos(void);
int get_pos(int, int);
int get_row(int);
void set_cursor_pos(int);
int vga_set_cell(char, char, int, int);

#endif /* _VGA_H */
