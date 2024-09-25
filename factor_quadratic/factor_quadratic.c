#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* macros */
#define SIGN(val) (val < 0) ? '-' : '+'
#define MAX_INPUT_LEN 30

/* function declarations */
static char *estrtok(char *str, const char *delim);
static inline void dieon(const int cond, const char *msg);
static int gcd(const int a, const int b);
static int strtoint(const char sign, const char *str);

/* function implementations */
static char *
estrtok(char *str, const char *delim)
{
	/* strtok except it fails instead of returning NULL */
	char *ptr = strtok(str, delim);
	dieon(ptr == NULL, "strtok failed\n");
	return ptr;
}

static inline void
dieon(const int cond, const char *msg)
{
	if (cond) {
		fputs(msg, stderr);
		exit(EXIT_FAILURE);
	}
}

static int
gcd(const int a, const int b)
{
	if (b)
		return gcd(b, a % b); /* yucky recursion */
	return a;
}

static int
strtoint(const char sign, const char *str)
{
	int i = 0, res = 0, mul = (sign == '-') ? -1 : 1;

	for (; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';

	return res * mul;
}

int
main(int argc, char *argv[])
{
	/* variable declarations */
	char input[MAX_INPUT_LEN];
	char eq[5][MAX_INPUT_LEN];
	char *ptr;

	int lead, mid, cons, mu, j, gcd1;
	int i = 0, found = 0;

	/* actually doing stuff */
	dieon(argc != 2, "need 1 argument\n");
	dieon(strlen(argv[1]) >= MAX_INPUT_LEN, "argument too long\n");

	strcpy(input, argv[1]);
	ptr = estrtok(input, " ");

	for (; ptr != NULL; ++i) {
		strcpy(eq[i], ptr);
		ptr = strtok(NULL, " ");
	}
	
	lead = strtoint(eq[0][0], estrtok(&eq[0][eq[0][0] == '-'], "x"));
	mid = strtoint(eq[1][0], estrtok(eq[2], "x"));
	cons = strtoint(eq[3][0], eq[4]);

	mu = abs(lead * cons);
	for (i = -mu; i <= mu; i++) {
		for (j = -mu; j <= mu; j++) {
			if ((i + j == mid) && (i * j == lead * cons)) {
				found = 1;
				break;
			}
		}

		if (found) break;
	}
	dieon(!found, "couldnt find factor pairs, equation is probably unfactorable\n");

	gcd1 = abs(gcd(lead, i));
	printf("(%dx %c %d)(%dx %c %d)\n", lead / gcd1, SIGN(i), abs(i / gcd1), gcd1, SIGN((eq[0][0] == '-') ? lead : j), abs(gcd(j, cons)));
	return EXIT_SUCCESS;
}
