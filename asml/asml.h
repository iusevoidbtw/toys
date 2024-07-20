/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

#if !defined(ASML_H)
#define ASML_H
#include <stdint.h>

extern unsigned int asml_winw;
extern unsigned int asml_winh;

/* constant definitions */
enum asml_event {
	ASML_NOEVENT,
	ASML_QUIT,
	ASML_RESIZE,
	ASML_KEYDOWN,
	ASML_KEYUP,
	ASML_MOUSEDOWN,
	ASML_MOUSEMOTION
};

struct asml_event_info {
	/* ASML_KEY(DOWN|UP) */
	const char *key;
	/* ASML_MOUSEDOWN */
	unsigned int button;
	int x;
	int y;
};

/* drawing functions */
void asml_clearscr(void);
void asml_drawcircle(int, int, int);
void asml_drawline(int, int, int, int);
void asml_drawpixel(int, int);
void asml_drawrect(int, int, int, int);
void asml_fillrect(int, int, int, int);
void asml_flush(void);
void asml_setdrawcolor(uint8_t, uint8_t, uint8_t);

/* general functions */
int asml_init(unsigned int, unsigned int, const char *);
enum asml_event asml_waitevent(struct asml_event_info *);
void asml_quit(void);
#endif /* ASML_H */

#if defined(ASML_IMPLEMENTATION)

#if !defined(ASML_BACKEND_X11) && !defined(ASML_BACKEND_WAYLAND)
#error please define an ASML backend.
#endif /* !ASML_BACKEND_X11 && !ASML_BACKEND_WAYLAND */

#if defined(ASML_BACKEND_X11) && defined(ASML_BACKEND_WAYLAND)
#error cannot have more than one ASML backend defined.
#endif /* ASML_BACKEND_X11 && ASML_BACKEND_WAYLAND */

#if defined(ASML_BACKEND_X11)

#include <X11/Xlib.h>

#include <stdlib.h>
#include <string.h>

/* macros */
#define asml_internal_min(x, y) (x < y) ? x : y

/* global variables */
static int asml_internal_closed = 0;

static Display *asml_internal_dpy;
static Window asml_internal_win;
static GC asml_internal_gc;
static unsigned long asml_internal_black;
static unsigned long asml_internal_white;
static unsigned long asml_internal_ucolor;

unsigned int asml_winw;
unsigned int asml_winh;

/*
 * ===========================================================================
 * drawing functions.
 */
void
asml_clearscr(void)
{
	XSetForeground(asml_internal_dpy, asml_internal_gc,
			asml_internal_black);
	XFillRectangle(asml_internal_dpy, asml_internal_win,
			asml_internal_gc, 0, 0,
			asml_winw, asml_winh);
	XSetForeground(asml_internal_dpy, asml_internal_gc,
			asml_internal_ucolor);
}

void
asml_drawcircle(int xm, int ym, int r)
{
	XDrawArc(asml_internal_dpy, asml_internal_win,
			asml_internal_gc,
			xm - r, ym - r, r / 2, r / 2,
			0, 360 * 64);
}

void
asml_drawline(int x1, int y1, int x2, int y2)
{
	XDrawLine(asml_internal_dpy, asml_internal_win,
			asml_internal_gc, x1, y1, x2, y2);
}

void
asml_drawpixel(int x, int y)
{
	XDrawPoint(asml_internal_dpy, asml_internal_win,
			asml_internal_gc, x, y);
}

void
asml_drawrect(int x1, int y1, int x2, int y2)
{
	XDrawRectangle(asml_internal_dpy, asml_internal_win,
			asml_internal_gc, asml_internal_min(x1, x2),
			asml_internal_min(y1, y2), (unsigned)abs(x2 - x1),
			(unsigned)abs(y2 - y1));
}

void
asml_fillrect(int x1, int y1, int x2, int y2)
{
	XFillRectangle(asml_internal_dpy, asml_internal_win,
			asml_internal_gc, asml_internal_min(x1, x2), asml_internal_min(y1, y2),
			(unsigned)abs(x2 - x1),
			(unsigned)abs(y2 - y1));
}

void
asml_flush(void)
{
	XFlush(asml_internal_dpy);
}

void
asml_setdrawcolor(uint8_t r, uint8_t g, uint8_t b)

	static int unfreed = 0;
	Colormap colormap = DefaultColormap(asml_internal_dpy,
			DefaultScreen(asml_internal_dpy));
	if (unfreed) {
		XFreeColors(asml_internal_dpy, colormap,
				&asml_internal_ucolor,
				1, 0);
	}
	if (r == 0 && g == 0 && b == 0) {
		unfreed = 0;
		asml_internal_ucolor = asml_internal_black;
		XSetForeground(asml_internal_dpy, asml_internal_gc,
				asml_internal_black);
	} else if (r == 255 && g == 255 && b == 255) {
		unfreed = 0;
		asml_internal_ucolor = asml_internal_white;
		XSetForeground(asml_internal_dpy, asml_internal_gc,
				asml_internal_white);
	} else {
		XColor color;
		color.red = r * 257;
		color.green = g * 257;
		color.blue = b * 257;
		XAllocColor(asml_internal_dpy, colormap,
				&color);

		asml_internal_ucolor = color.pixel;
		XSetForeground(asml_internal_dpy, asml_internal_gc,
				asml_internal_ucolor);
	}
}

/*
 * ===========================================================================
 * general functions
 */
int
asml_init(unsigned int w, unsigned int h, const char *title)
{
	Atom wm_delete;

	/* open display */
	asml_internal_dpy = XOpenDisplay(NULL);
	if (asml_internal_dpy == NULL) {
		return -1;
	}

	/* define colors */
	asml_internal_black = BlackPixel(asml_internal_dpy, DefaultScreen(asml_internal_dpy));
	asml_internal_white = WhitePixel(asml_internal_dpy, DefaultScreen(asml_internal_dpy));

	/* create window */
	asml_internal_win = XCreateSimpleWindow(asml_internal_dpy,
			DefaultRootWindow(asml_internal_dpy), 0, 0, w, h, 0,
			asml_internal_black, asml_internal_black);
	asml_winw = w;
	asml_winh = h;

	/* name our window */
	XStoreName(asml_internal_dpy, asml_internal_win, title);

	/* specify WM_DELETE_WINDOW protocol */
	wm_delete = XInternAtom(asml_internal_dpy, "WM_DELETE_WINDOW", 1);
	XSetWMProtocols(asml_internal_dpy, asml_internal_win, &wm_delete, 1);

	/* tell X what events we are interested in */
	XSelectInput(asml_internal_dpy, asml_internal_win,
			StructureNotifyMask | KeyPressMask | KeyReleaseMask);

	/* map window */
	XMapWindow(asml_internal_dpy, asml_internal_win);

	/* create graphics context */
	asml_internal_gc = XCreateGC(asml_internal_dpy, asml_internal_win, 0, NULL);

	/* tell our GC we draw with the white color */
	asml_internal_ucolor = asml_internal_white;
	XSetForeground(asml_internal_dpy, asml_internal_gc, asml_internal_white);

	/* wait for a MapNotify event */
	for (;;) {
		XEvent xevnt;
		XNextEvent(asml_internal_dpy, &xevnt);
		if (xevnt.type == MapNotify) {
			break;
		}
	}

	XGrabPointer(asml_internal_dpy, asml_internal_win, False,
			ButtonPressMask | PointerMotionMask, GrabModeAsync,
			GrabModeAsync, None, None, CurrentTime);
	return 0;
}

enum asml_event
asml_waitevent(struct asml_event_info *event)
{
	static int prevx = -1, prevy = -1;

	XEvent xevnt;
	for (;;) {
		/* wait for an X event to happen. */
		XNextEvent(asml_internal_dpy, &xevnt);

		/* what type of event? */
		switch (xevnt.type) {
			/* we got a message */
			case ClientMessage:
				{
					char *ptr = XGetAtomName(asml_internal_dpy,
							xevnt.xclient.message_type);

					/*
					 * if the message is WM_PROTOCOLS,
					 * the user has closed the window.
					 */
					if (strcmp(ptr, "WM_PROTOCOLS") == 0) {
						XFree(ptr);
						return ASML_QUIT;
					}
					XFree(ptr);
					break;
				}
			/* the window has changed somehow. */
			case ConfigureNotify:
				if ((unsigned int)xevnt.xconfigure.width != asml_winw ||
						(unsigned int)xevnt.xconfigure.height != asml_winh) {
					asml_winw = (unsigned int)xevnt.xconfigure.width;
					asml_winh = (unsigned int)xevnt.xconfigure.height;
					return ASML_RESIZE;
				}
				break;
			case KeyPress:
				{
					KeySym ks = XLookupKeysym(&xevnt.xkey, 0);
					event->key = XKeysymToString(ks);
					return ASML_KEYDOWN;
				}
			case KeyRelease:
				{
					KeySym ks;
					if (XEventsQueued(asml_internal_dpy, QueuedAfterReading)) {
						XEvent xnext;
						XPeekEvent(asml_internal_dpy, &xnext);
						if (xnext.type == KeyPress &&
								xnext.xkey.time == xevnt.xkey.time &&
								xnext.xkey.keycode == xevnt.xkey.keycode) {
							XNextEvent(asml_internal_dpy, &xevnt);
							break;
						}
					}
					ks = XLookupKeysym(&xevnt.xkey, 0);
					event->key = XKeysymToString(ks);
					return ASML_KEYUP;
				}
			case ButtonPress:
				if (xevnt.xbutton.x >= 0 &&
						xevnt.xbutton.y >= 0) {
					event->button = xevnt.xbutton.button;
					event->x = xevnt.xbutton.x;
					event->y = xevnt.xbutton.y;
					return ASML_MOUSEDOWN;
				}
				break;
			case MotionNotify:
				/* TODO */
				if (xevnt.xmotion.x >= 0 &&
						xevnt.xmotion.y >= 0 &&
						(first || )) {
					event->x = xevnt.xmotion.x;
					event->y = xevnt.xmotion.y;
					return ASML_MOUSEMOTION;
				}
				break;
		}
	}
}

void
asml_quit(void)
{
	if (!asml_internal_closed) {
		XDestroyWindow(asml_internal_dpy, asml_internal_win);
		XCloseDisplay(asml_internal_dpy);
		asml_internal_closed = 1;
	}
}

#elif defined(ASML_BACKEND_WAYLAND)

/* this is where the boilerplate starts. */
#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "xdg-shell-client-protocol.h"

#define asml_internal_min(x, y) (x < y) ? x : y
#define asml_internal_max(x, y) (x > y) ? x : y
#define asml_internal_inline_pixelset(x, y) \
	if (x >= 0 && y >= 0) {\
		asml_internal_uframe[(y) * (int)(asml_internal_uframe_width) + (x)] = asml_internal_ucolor;\
	}

static int asml_internal_closed = 0;

static uint32_t *asml_internal_uframe;
static uint32_t asml_internal_ucolor = 0xFFFFFFFF;
static unsigned int asml_internal_uframe_width;
static unsigned int asml_internal_uframe_height;
static unsigned int asml_internal_uframe_stride;
static unsigned int asml_internal_uframe_size;

static enum asml_event curr_event = ASML_NOEVENT;
static char curr_event_key[128];
unsigned int asml_winw;
unsigned int asml_winh;
unsigned int curr_event_button;
int curr_event_x, curr_event_y;

/*
 * ===========================================================================
 * internal utility functions
 */
static void
asml_internal_frametrimcpy(uint32_t *dst, const uint32_t *src,
		unsigned int oldwidth, unsigned int newwidth,
		unsigned int oldstride, unsigned int newstride,
		unsigned int oldheight, unsigned int newheight)
{
	unsigned int roff = 0, woff = 0;
	unsigned int stride = asml_internal_min(oldstride, newstride);
	unsigned int y;
	for (y = 0; y < asml_internal_min(oldheight, newheight) &&
			(roff < oldwidth * oldheight) &&
			(woff < newwidth * newheight); y++) {
		memcpy(dst + woff, src + roff, stride);
		roff += oldwidth;
		woff += newwidth;
	}
}

/*
 * ===========================================================================
 * shared memory stuff.
 * i could probably use the memfd_create(2) syscall instead
 * but that's linux-only
 */
static void
randname(char *buf)
{
	/* generate a (pretty bad) random filename. */
	struct timespec ts;
	long r;
	clock_gettime(CLOCK_REALTIME, &ts);
	r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = (char)('A'+(r&15)+(r&16)*2);
		r >>= 5;
	}
}

static int
create_shm_file(void)
{
	int retries = 100;
	int fd;
	do {
		char name[] = "/asml-wl_shm-XXXXXX";
		randname(name + sizeof(name) - 7);
		retries--;
		fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			shm_unlink(name);
			return fd;
		}
	} while (retries > 0 && errno == EEXIST);
	return -1;
}

static int
allocate_shm_file(size_t size)
{
	int fd = create_shm_file();
	int ret;
	if (fd < 0) {
		return -1;
	}
	do {
		/*
		 * despite what it sounds like, ftruncate can
		 * actually increase the size of the file,
		 * that's what we're doing here
		 */
		ret = ftruncate(fd, (off_t)size);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0) {
		close(fd);
		return -1;
	}
	return fd;
}

/*
 * ===========================================================================
 * some wayland event handling code.
 * note: 'pointer' means whatever is controlling the cursor,
 * like your mouse, touchpad or graphics tablet.
 * a 'surface' is basically a Wayland window.
 */
enum pointer_event_mask {
	POINTER_EVENT_ENTER = 1 << 0,
	POINTER_EVENT_LEAVE = 1 << 1,
	POINTER_EVENT_MOTION = 1 << 2,
	POINTER_EVENT_BUTTON = 1 << 3,
	POINTER_EVENT_AXIS = 1 << 4,
	POINTER_EVENT_AXIS_SOURCE = 1 << 5,
	POINTER_EVENT_AXIS_STOP = 1 << 6,
	POINTER_EVENT_AXIS_DISCRETE = 1 << 7,
};

struct pointer_event {
	uint32_t event_mask;
	wl_fixed_t surface_x, surface_y;
	uint32_t button, state;
	uint32_t time;
	uint32_t serial;
	struct {
		int valid;
		wl_fixed_t value;
		int32_t discrete;
	} axes[2];
	uint32_t axis_source;
};

struct client_state {
	/* globals */
	struct wl_display *wl_display;
	struct wl_registry *wl_registry;
	struct wl_shm *wl_shm;
	struct wl_compositor *wl_compositor;
	struct xdg_wm_base *xdg_wm_base;
	struct wl_seat *wl_seat;
	/* objects */
	struct wl_surface *wl_surface;
	struct xdg_surface *xdg_surface;
	struct wl_keyboard *wl_keyboard;
	struct wl_pointer *wl_pointer;
	struct wl_touch *wl_touch;
	struct xdg_toplevel *xdg_toplevel;
	/* state */
	uint32_t last_frame;
	unsigned int width, height;
	struct pointer_event pointer_event;
	struct xkb_state *xkb_state;
	struct xkb_context *xkb_context;
	struct xkb_keymap *xkb_keymap;
};

static void
wl_pointer_enter(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, __attribute__((__unused__)) struct wl_surface *surface,
		wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	/* pointer entered our surface */
	struct client_state *client_state = (struct client_state *)data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_ENTER;
	client_state->pointer_event.serial = serial;
	client_state->pointer_event.surface_x = surface_x,
		client_state->pointer_event.surface_y = surface_y;
}

static void
wl_pointer_leave(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, __attribute__((__unused__)) struct wl_surface *surface)
{
	/* pointer left our surface */
	struct client_state *client_state = (struct client_state *)data;
	client_state->pointer_event.serial = serial;
	client_state->pointer_event.event_mask |= POINTER_EVENT_LEAVE;
}

static void
wl_pointer_motion(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	struct client_state *client_state = (struct client_state *)data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_MOTION;
	client_state->pointer_event.time = time;
	client_state->pointer_event.surface_x = surface_x,
		client_state->pointer_event.surface_y = surface_y;
}

static void
wl_pointer_button(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	struct client_state *client_state = (struct client_state *)data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_BUTTON;
	client_state->pointer_event.time = time;
	client_state->pointer_event.serial = serial;
	client_state->pointer_event.button = button,
		client_state->pointer_event.state = state;
}

static void
wl_pointer_axis(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, uint32_t axis, wl_fixed_t value)
{
	struct client_state *client_state = data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS;
	client_state->pointer_event.time = time;
	client_state->pointer_event.axes[axis].valid = 1;
	client_state->pointer_event.axes[axis].value = value;
}

static void
wl_pointer_axis_source(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t axis_source)
{
	struct client_state *client_state = data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_SOURCE;
	client_state->pointer_event.axis_source = axis_source;
}

static void
wl_pointer_axis_stop(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, uint32_t axis)
{
	struct client_state *client_state = data;
	client_state->pointer_event.time = time;
	client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_STOP;
	client_state->pointer_event.axes[axis].valid = 1;
}

static void
wl_pointer_axis_discrete(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t axis, int32_t discrete)
{
	struct client_state *client_state = data;
	client_state->pointer_event.event_mask |= POINTER_EVENT_AXIS_DISCRETE;
	client_state->pointer_event.axes[axis].valid = 1;
	client_state->pointer_event.axes[axis].discrete = discrete;
}

static void
wl_pointer_frame(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer)
{
	/* some pointer event happened, this is where we handle these */
	struct client_state *client_state = (struct client_state *)data;
	struct pointer_event *event = &client_state->pointer_event;

	if ((event->event_mask & POINTER_EVENT_ENTER) ||
			(event->event_mask & POINTER_EVENT_MOTION)) {
		curr_event_x = wl_fixed_to_int(event->surface_x);
		curr_event_y = wl_fixed_to_int(event->surface_y);
		curr_event = ASML_MOUSEMOTION;
	}

	/*
	if (event->event_mask & POINTER_EVENT_LEAVE) {
		fprintf(stderr, "leave");
	}
	*/

	if (event->event_mask & POINTER_EVENT_BUTTON) {
		if (event->state != WL_POINTER_BUTTON_STATE_RELEASED) {
			curr_event = ASML_MOUSEDOWN;
			switch (event->button) {
				case 272:
					curr_event_button = 1;
					break;
				case 273:
					curr_event_button = 3;
					break;
				case 274:
					curr_event_button = 2;
					break;
				case 275:
					curr_event_button = 4;
					break;
				case 276:
					curr_event_button = 5;
					break;
			}
		}
	}

	/*
	uint32_t axis_events = POINTER_EVENT_AXIS
		| POINTER_EVENT_AXIS_SOURCE
		| POINTER_EVENT_AXIS_STOP
		| POINTER_EVENT_AXIS_DISCRETE;
	char *axis_name[2] = {
		[WL_POINTER_AXIS_VERTICAL_SCROLL] = "vertical",
		[WL_POINTER_AXIS_HORIZONTAL_SCROLL] = "horizontal",
	};
	char *axis_source[4] = {
		[WL_POINTER_AXIS_SOURCE_WHEEL] = "wheel",
		[WL_POINTER_AXIS_SOURCE_FINGER] = "finger",
		[WL_POINTER_AXIS_SOURCE_CONTINUOUS] = "continuous",
		[WL_POINTER_AXIS_SOURCE_WHEEL_TILT] = "wheel tilt",
	};
	if (event->event_mask & axis_events) {
		for (size_t i = 0; i < 2; ++i) {
			if (!event->axes[i].valid) {
				continue;
			}
			fprintf(stderr, "%s axis ", axis_name[i]);
			if (event->event_mask & POINTER_EVENT_AXIS) {
				fprintf(stderr, "value %f ", wl_fixed_to_double(
							event->axes[i].value));
			}
			if (event->event_mask & POINTER_EVENT_AXIS_DISCRETE) {
				fprintf(stderr, "discrete %d ",
						event->axes[i].discrete);
			}
			if (event->event_mask & POINTER_EVENT_AXIS_SOURCE) {
				fprintf(stderr, "via %s ",
						axis_source[event->axis_source]);
			}
			if (event->event_mask & POINTER_EVENT_AXIS_STOP) {
				fputs("(stopped) ", stderr);
			}
		 }
		putc('\n', stderr);
	}
	*/

	memset(event, 0, sizeof(*event));
}

static const struct wl_pointer_listener wl_pointer_listener = {
	.enter = wl_pointer_enter,
	.leave = wl_pointer_leave,
	.motion = wl_pointer_motion,
	.button = wl_pointer_button,
	.axis = wl_pointer_axis,
	.frame = wl_pointer_frame,
	.axis_source = wl_pointer_axis_source,
	.axis_stop = wl_pointer_axis_stop,
	.axis_discrete = wl_pointer_axis_discrete,
};

/* keyboard */
static void
wl_keyboard_keymap(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		uint32_t format, int32_t fd, uint32_t size)
{
	struct client_state *client_state = (struct client_state *)data;
	char *map_shm;
	struct xkb_keymap *xkb_keymap;
	struct xkb_state *xkb_state;

	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
		/* TODO: fix this */
		fputs("asml: unsupported keymap format, this will be fixed later\n", stderr);
		exit(1);
	}

	map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (map_shm == MAP_FAILED) {
		fprintf(stderr, "asml: mmap failed: %s\n", strerror(errno));
		exit(1);
	}

	/* configure the keymap */
	xkb_keymap = xkb_keymap_new_from_string(client_state->xkb_context, map_shm,
			XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap(map_shm, size);
	close(fd);

	xkb_state = xkb_state_new(xkb_keymap);
	xkb_keymap_unref(client_state->xkb_keymap);
	xkb_state_unref(client_state->xkb_state);
	client_state->xkb_keymap = xkb_keymap;
	client_state->xkb_state = xkb_state;
}

static void
wl_keyboard_enter(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) struct wl_surface *surface,
		__attribute__((__unused__)) struct wl_array *keys)
{
	/* TODO: fire ASML_KEYDOWN event for each key */
	/*
	struct client_state *client_state = data;
	fputs("keyboard enter; keys pressed are:\n", stderr);
	uint32_t *key;
	wl_array_for_each(key, keys) {
		char buf[128];
		xkb_keysym_t sym = xkb_state_key_get_one_sym(
				client_state->xkb_state, *key + 8);
		xkb_keysym_get_name(sym, buf, sizeof(buf));
		fprintf(stderr, "sym: %-12s (%d), ", buf, sym);
		xkb_state_key_get_utf8(client_state->xkb_state,
				*key + 8, buf, sizeof(buf));
		fprintf(stderr, "utf8: '%s'\n", buf);
	}
	*/
}

static void
wl_keyboard_key(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) uint32_t time, uint32_t key, uint32_t state)
{
	/* key press/release event */
	struct client_state *client_state = (struct client_state *)data;
	char buf[128];
	xkb_keysym_t sym = xkb_state_key_get_one_sym(
			client_state->xkb_state, key + 8);
	xkb_keysym_get_name(sym, buf, sizeof(buf));
	if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		curr_event = ASML_KEYDOWN;
	} else {
		curr_event = ASML_KEYUP;
	}
	memcpy(curr_event_key, buf, sizeof(buf));
}

static void
wl_keyboard_leave(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) struct wl_surface *surface)
{
	/*fputs("keyboard leave\n", stderr);*/
}

static void
wl_keyboard_modifiers(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial, uint32_t mods_depressed,
		uint32_t mods_latched, uint32_t mods_locked,
		uint32_t group)
{
	struct client_state *client_state = (struct client_state *)data;
	xkb_state_update_mask(client_state->xkb_state,
			mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void
wl_keyboard_repeat_info(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) int32_t rate,
		__attribute__((__unused__)) int32_t delay)
{
	/* TODO */
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
	.keymap = wl_keyboard_keymap,
	.enter = wl_keyboard_enter,
	.leave = wl_keyboard_leave,
	.key = wl_keyboard_key,
	.modifiers = wl_keyboard_modifiers,
	.repeat_info = wl_keyboard_repeat_info,
};

static void
wl_buffer_release(__attribute__((__unused__)) void *data, struct wl_buffer *wl_buffer)
{
	/* sent by the compositor when it's no longer using this buffer */
	wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
	.release = wl_buffer_release,
};

static struct wl_buffer *
draw_frame(struct client_state *state)
{
	unsigned int width = state->width, height = state->height;
	unsigned int stride = width * 4;
	unsigned int size = stride * height;

	int fd = allocate_shm_file(size);
	uint32_t *data;
	struct wl_shm_pool *pool;
	struct wl_buffer *buffer;

	if (fd == -1) {
		return NULL;
	}

	/*
	 * notice the MAP_SHARED flag. this is because we (obviously)
	 * want others to be able to read the contents of data
	 */
	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		fputs("asml: mmap failed, not drawing this frame\n", stderr);
		close(fd);
		return NULL;
	}

	/* create a Wayland buffer object */
	pool = wl_shm_create_pool(state->wl_shm, fd, (int32_t)size);
	buffer = wl_shm_pool_create_buffer(pool, 0, (int32_t)width, (int32_t)height,
			(int32_t)stride, WL_SHM_FORMAT_XRGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	if (width != asml_internal_uframe_width || height != asml_internal_uframe_height) {
		/* the uframe buffer needs resizing */
		uint32_t *nasml_internal_uframe = malloc(size);
		if (nasml_internal_uframe == NULL) {
			fputs("asml: malloc failed, not drawing this frame\n", stderr);
			munmap(data, size);
			return NULL;
		}
		asml_internal_frametrimcpy(nasml_internal_uframe, asml_internal_uframe,
			asml_internal_uframe_width, width,
			asml_internal_uframe_stride, stride,
			asml_internal_uframe_height, height);
		free(asml_internal_uframe);
		asml_internal_uframe = nasml_internal_uframe;
		asml_internal_uframe_width = width;
		asml_internal_uframe_stride = stride;
		asml_internal_uframe_height = height;
	}
	memcpy(data, asml_internal_uframe, asml_internal_uframe_size);

	munmap(data, size);
	wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
	return buffer;
}

static void
xdg_toplevel_configure(void *data,
		__attribute__((__unused__)) struct xdg_toplevel *xdg_toplevel,
		int32_t width, int32_t height, __attribute__((__unused__)) struct wl_array *states)
{
	struct client_state *state = (struct client_state *)data;
	if (width == 0 || height == 0) {
		/* compositor is deferring to us */
		return;
	}
	if (width != (int32_t)asml_winw || height != (int32_t)asml_winh) {
		state->width = asml_winw = (unsigned int)width;
		state->height = asml_winh = (unsigned int)height;
		curr_event = ASML_RESIZE;
	}
}

static void
xdg_toplevel_close(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct xdg_toplevel *toplevel)
{
	curr_event = ASML_QUIT;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
	.configure = xdg_toplevel_configure,
	.close = xdg_toplevel_close,
};

static void
xdg_surface_configure(void *data,
		struct xdg_surface *xdg_surface, uint32_t serial)
{
	struct client_state *state = (struct client_state *)data;
	struct wl_buffer *buffer;
	xdg_surface_ack_configure(xdg_surface, serial);

	buffer = draw_frame(state);
	wl_surface_attach(state->wl_surface, buffer, 0, 0);
	wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};

static void
xdg_wm_base_ping(__attribute__((__unused__)) void *data,
		struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
	xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

static void
wl_seat_capabilities(void *data, __attribute__((__unused__)) struct wl_seat *wl_seat,
		uint32_t capabilities)
{
	struct client_state *state = (struct client_state *)data;
	int have_keyboard;

	int have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

	if (have_pointer && state->wl_pointer == NULL) {
		state->wl_pointer = wl_seat_get_pointer(state->wl_seat);
		wl_pointer_add_listener(state->wl_pointer,
				&wl_pointer_listener, state);
	} else if (!have_pointer && state->wl_pointer != NULL) {
		wl_pointer_release(state->wl_pointer);
		state->wl_pointer = NULL;
	}

	have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

	if (have_keyboard && state->wl_keyboard == NULL) {
		state->wl_keyboard = wl_seat_get_keyboard(state->wl_seat);
		wl_keyboard_add_listener(state->wl_keyboard,
				&wl_keyboard_listener, state);
	} else if (!have_keyboard && state->wl_keyboard != NULL) {
		wl_keyboard_release(state->wl_keyboard);
		state->wl_keyboard = NULL;
	}
}

static void
wl_seat_name(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_seat *wl_seat,
		__attribute__((__unused__)) const char *name)
{
	/*fprintf(stderr, "seat name: %s\n", name);*/
}

static const struct wl_seat_listener wl_seat_listener = {
	.capabilities = wl_seat_capabilities,
	.name = wl_seat_name,
};

static void
registry_global(void *data, struct wl_registry *wl_registry,
		uint32_t name, const char *interface,
		__attribute__((__unused__)) uint32_t version)
{
	struct client_state *state = (struct client_state *)data;
	if (strcmp(interface, wl_shm_interface.name) == 0) {
		state->wl_shm = wl_registry_bind(
				wl_registry, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		state->wl_compositor = wl_registry_bind(
				wl_registry, name, &wl_compositor_interface, 4);
	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		state->xdg_wm_base = wl_registry_bind(
				wl_registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(state->xdg_wm_base,
				&xdg_wm_base_listener, state);
	} else if (strcmp(interface, wl_seat_interface.name) == 0) {
		state->wl_seat = wl_registry_bind(
				wl_registry, name, &wl_seat_interface, 7);
		wl_seat_add_listener(state->wl_seat,
				&wl_seat_listener, state);
	}
}

static void
registry_global_remove(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_registry *wl_registry,
		__attribute__((__unused__)) uint32_t name)
{
	/* stub */
}

static const struct wl_registry_listener wl_registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};
	
static struct client_state mainstate = { 0 };

/*
 * ===========================================================================
 * drawing functions
 */
void
asml_clearscr(void)
{
	memset(asml_internal_uframe, 0, asml_internal_uframe_size);
}

void
asml_drawcircle(int xm, int ym, int r)
{
	int x = -r, y = 0, err = 2 - 2 * r;
	do {
		asml_internal_inline_pixelset(xm - x, ym + y);
		asml_internal_inline_pixelset(xm - y, ym - x);
		asml_internal_inline_pixelset(xm + x, ym - y);
		asml_internal_inline_pixelset(xm + y, ym + x);

		r = err;
		if (r <= y) {
			err += ++y * 2 + 1;
		}
		if (r > x || err > y) {
			err += ++x * 2 + 1;
		}
	} while (x < 0);
}

void
asml_drawline(int x1, int y1, int x2, int y2)
{
	if (x1 == x2) {
		int lowy = asml_internal_min(y1, y2);
		int highy = asml_internal_max(y1, y2);
		for (; lowy <= highy; lowy++) {
			asml_internal_inline_pixelset(x1, lowy);
		}
	} else if (y1 == y2) {
		int lowx = asml_internal_min(x1, x2);
		int highx = asml_internal_max(x1, x2);
		for (; lowx <= highx; lowx++) {
			asml_internal_inline_pixelset(lowx, y1);
		}
	} else {
		int dx = abs(x2 - x1), sx = (x1 < x2) ? 1 : -1;
		int dy = -abs(y2 - y1), sy = (y1 < y2) ? 1 : -1;
		int err = dx + dy, e2;

		for (;;) {
			asml_internal_inline_pixelset(x1, y1);
			if (x1 == x2 && y1 == y2) {
				break;
			}
			e2 = err * 2;
			if (e2 >= dy) {
				err += dy;
				x1 += sx;
			}
			if (e2 <= dx) {
				err += dx;
				y1 += sy;
			}
		}
	}
}

void
asml_drawpixel(int x, int y)
{
	asml_internal_inline_pixelset(x, y);
}

void
asml_drawrect(int x1, int y1, int x2, int y2)
{
	asml_drawline(x1, y1, x2, y1); /* top */
	asml_drawline(x2, y1, x2, y2); /* right */
	asml_drawline(x1, y2, x2, y2); /* bottom */
	asml_drawline(x1, y1, x1, y2); /* left */
}

void
asml_fillrect(int x1, int y1, int x2, int y2)
{
	int x;
	int lowx = asml_internal_min(x1, x2);
	int highx = asml_internal_max(x1, x2);
	int lowy = asml_internal_min(y1, y2);
	int highy = asml_internal_max(y1, y2);

	for (; lowy <= highy; lowy++) {
		for (x = lowx; x <= highx; x++) {
			asml_internal_inline_pixelset(x, lowy);
		}
	}
}

void
asml_flush(void)
{
	/* submit a frame for this event */
	if (!asml_internal_closed) {
		struct wl_buffer *buffer = draw_frame(&mainstate);
		wl_surface_attach(mainstate.wl_surface, buffer, 0, 0);
		wl_surface_damage_buffer(mainstate.wl_surface, 0, 0, INT32_MAX, INT32_MAX);
		wl_surface_commit(mainstate.wl_surface);
	}
}

void
asml_setdrawcolor(uint8_t r, uint8_t g, uint8_t b)
{
	/* remember we're using XRGB */
	asml_internal_ucolor = (uint32_t)((0xFF << 24) | (r << 16) | (g << 8) | (b));
}


/*
 * ===========================================================================
 * general functions
 */
int
asml_init(unsigned int w, unsigned int h, const char *title)
{
	mainstate.width = asml_internal_uframe_width = asml_winw = w;
	mainstate.height = asml_internal_uframe_height = asml_winh = h;
	asml_internal_uframe_stride = asml_internal_uframe_width * 4;
	asml_internal_uframe_size = asml_internal_uframe_stride * asml_internal_uframe_height;

	asml_internal_uframe = malloc(asml_internal_uframe_size);
	if (asml_internal_uframe == NULL) {
		return -1;
	}

	mainstate.wl_display = wl_display_connect(NULL);
	mainstate.wl_registry = wl_display_get_registry(mainstate.wl_display);
	mainstate.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	wl_registry_add_listener(mainstate.wl_registry, &wl_registry_listener, &mainstate);
	wl_display_roundtrip(mainstate.wl_display);

	mainstate.wl_surface = wl_compositor_create_surface(mainstate.wl_compositor);
	mainstate.xdg_surface = xdg_wm_base_get_xdg_surface(
			mainstate.xdg_wm_base, mainstate.wl_surface);
	xdg_surface_add_listener(mainstate.xdg_surface, &xdg_surface_listener, &mainstate);
	mainstate.xdg_toplevel = xdg_surface_get_toplevel(mainstate.xdg_surface);
	xdg_toplevel_add_listener(mainstate.xdg_toplevel,
			&xdg_toplevel_listener, &mainstate);
	xdg_toplevel_set_title(mainstate.xdg_toplevel, title);
	wl_surface_commit(mainstate.wl_surface);

	return 0;
}

enum asml_event
asml_waitevent(struct asml_event_info *event)
{
	for (;;) {
		/* wait for a Wayland event to happen. */
		wl_display_dispatch(mainstate.wl_display);
		switch (curr_event) {
			case ASML_QUIT:
				curr_event = ASML_NOEVENT;
				return ASML_QUIT;
			case ASML_RESIZE:
				curr_event = ASML_NOEVENT;
				return ASML_RESIZE;
			case ASML_KEYDOWN:
				event->key = curr_event_key;
				curr_event = ASML_NOEVENT;
				return ASML_KEYDOWN;
			case ASML_KEYUP:
				event->key = curr_event_key;
				curr_event = ASML_NOEVENT;
				return ASML_KEYUP;
			case ASML_MOUSEDOWN:
				event->x = curr_event_x;
				event->y = curr_event_y;
				event->button = curr_event_button;
				curr_event = ASML_NOEVENT;
				return ASML_MOUSEDOWN;
			case ASML_MOUSEMOTION:
				event->x = curr_event_x;
				event->y = curr_event_y;
				curr_event = ASML_NOEVENT;
				return ASML_MOUSEMOTION;
			default:
				break;
		}
	}
}

void
asml_quit(void)
{
	if (!asml_internal_closed) {
		free(asml_internal_uframe);
		asml_internal_uframe = NULL;

		wl_surface_attach(mainstate.wl_surface, NULL, 0, 0);
		wl_surface_commit(mainstate.wl_surface);
		wl_surface_destroy(mainstate.wl_surface);
		xdg_surface_destroy(mainstate.xdg_surface);
		xdg_toplevel_destroy(mainstate.xdg_toplevel);
		xdg_wm_base_destroy(mainstate.xdg_wm_base);
		wl_display_disconnect(mainstate.wl_display);
		asml_internal_closed = 1;
	}
}

#endif /* ASML_BACKEND_X11 || ASML_BACKEND_WAYLAND */

#endif /* ASML_IMPLEMENTATION */
