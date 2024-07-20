#define ASML_IMPLEMENTATION
#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <stdio.h>
#include <time.h>

#include "../asml.h"

static int
msleep(long ms)
{
	/* sleep for ms milliseconds. */
	struct timespec ts;
	int ret;

	if (ms < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;

	do {
		ret = nanosleep(&ts, &ts);
	} while (ret && errno == EINTR);

	return ret;
}

int
main(void)
{
	/*
	 * create a 640x480 window.
	 *
	 * note that the actual size might be different sometimes
	 * (e.g when using a tiling window manager), so
	 * the window dimensions are stored in the asml_winw (width) and
	 * asml_winh (height) variables.
	 */
	struct asml_event_info event;
	unsigned int xpos = 0;
	unsigned int ypos = 200;
	int cont = 1;

	asml_init(640, 480, "example asml program");
	asml_setdrawcolor(0, 255, 0);

	while (cont) {
		switch (asml_waitevent(&event)) {
			case ASML_QUIT:
				cont = 0;
				break;
			case ASML_RESIZE:
				printf("window resized: width = %d height = %d\n",
						asml_winw, asml_winh);
				break;
			case ASML_KEYDOWN:
				/* note: lower Y position = higher */
				if (event.key[0] == 'w' && event.key[1] == '\0') {
					/* if W is pressed, move square up. */
					if (ypos == 0) {
						ypos = asml_winh;
					} else {
						ypos--;
					}
				} else if (event.key[0] == 's' && event.key[1] == '\0') {
					/* if S is pressed, move square down. */
					if (ypos == asml_winh) {
						ypos = 0;
					} else {
						ypos++;
					}
				}
				printf("key %s pressed\n", event.key);
				break;
			case ASML_KEYUP:
				printf("key %s released\n", event.key);
				break;
			case ASML_MOUSEDOWN:
				printf("mouse clicked: button = %u x = %d y = %d\n",
						event.button, event.x, event.y);
				break;
			case ASML_MOUSEMOTION:
				printf("mouse motion: x = %d y = %d\n", event.x, event.y);
				break;
			default:
				break;
		}

		/* move the square forward 20 times per second. */
		if (xpos == asml_winw) {
			/* wrap to avoid it going off the screen. */
			xpos = 0;
		} else {
			xpos++;
		}
		asml_clearscr();
		/*asml_drawrect((int)xpos, (int)ypos, (int)xpos + 50, (int)ypos + 50);*/
		asml_drawcircle((int)xpos, (int)ypos, 25);

		/* actually write the pixels to the screen */
		asml_flush();
		msleep(50);	/* 20 FPS */
	}
	asml_quit();
	puts("asml_quit");
	return 0;
}
