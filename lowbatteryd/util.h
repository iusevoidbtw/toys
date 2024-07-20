#ifndef UTIL_H
#define UTIL_H

/* err.c */
extern char *argv0;

void die(const char *fmt, ...);
void logerr(const char *fmt, ...);

/* strtonum.c */
long long strtonum(const char *numstr, long long minval, long long maxval,
		const char **errstrp);
long long estrtonum(const char *numstr, long long minval, long long maxval);

#endif
