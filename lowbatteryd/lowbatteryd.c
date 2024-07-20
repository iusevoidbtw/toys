#define _BSD_SOURCE
#include <sys/wait.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"

/* config */
#define CRITICAL        5
#define LOW             15

#define CRITICAL_NOTIFY "critical battery level"
#define LOW_NOTIFY      "low battery level"

#define BATTERY         "BAT1"
#define NOTIFICATION    "mayflower"
#define NOTIFY_FLAGS    "mayflower", "-d", "-0"
#define POWEROFF        "poweroff"

#define POLL_RATE       60

/* function declarations */
static void notification(const char *text);
static void poweroff(void);

/* function implementations */
static void
notification(const char *text)
{
	int wstatus;
	switch (vfork()) {
	case -1:
		logerr("vfork:");
		break;
	case 0:
		execlp(NOTIFICATION, NOTIFY_FLAGS, text, (char *)NULL);
		_exit(1);
	default:
		wait(&wstatus);
		if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0)
			poweroff();
	}
}

static void
poweroff(void)
{
	switch (vfork()) {
	case -1:
		logerr("vfork:");
		break;
	case 0:
		execlp(POWEROFF, POWEROFF, (char *)NULL);
		_exit(1);
	default:
		wait(NULL);
	}
}

/* main() */
int
main(void)
{
	char buf[4];
	int fd = open("/sys/class/power_supply/" BATTERY "/capacity", O_RDONLY);
	ssize_t nread;

	int capacity;

	if (fd < 0)
		die("open:");

	for (;;) {
		if ((nread = read(fd, buf, sizeof(buf) - 1)) < 0) {
			logerr("read:");
			continue;
		}

		buf[nread] = '\0';
		if (nread > 0 && buf[nread - 1] == '\n')
			buf[nread - 1] = '\0';
		
		capacity = estrtonum(buf, 0, 100);
		if (capacity < CRITICAL)
			notification(CRITICAL_NOTIFY);
		else if (capacity < LOW)
			notification(LOW_NOTIFY);
		sleep(POLL_RATE);
	}
	close(fd);
}
