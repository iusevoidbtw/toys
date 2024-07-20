#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bsd/string.h>

/* macros */
#define ABS(val) (val < 0) ? -(val) : (val)
#define SIGN(val) (val < 0) ? '-' : '+'
#define MAX_INPUT_LEN 30

/* function declarations */
void dieon(const int cond, const char *msg);
int gcd(const int a, const int b);
int strtoint(const char sign, const char *str);
char *sstrtok(char *str, const char *delim);

/* function implementations */
void
dieon(const int cond, const char *msg)
{
	if (cond) {
		fputs(msg, stderr);
		exit(EXIT_FAILURE);
	}
}

int
gcd(const int a, const int b)
{
	if (b) {
		return gcd(b, a % b); /* yucky recursion */
	}
	return a;
}

int
strtoint(const char sign, const char *str)
{
	int i, res, mul;
	mul = (sign == '-') ? -1 : 1;

	res = 0;
	for (i = 0; str[i] != '\0'; i++) {
		res = res * 10 + str[i] - '0';
	}

	return res * mul;
}

char *
sstrtok(char *str, const char *delim)
{
	/* strtok except it fails instead of returning NULL */
	char *ptr = strtok(str, delim);
	dieon(ptr == NULL, "strtok failed\n");
	return ptr;
}

int
main(int argc, char *argv[])
{
	/* variable declarations */
	char input[MAX_INPUT_LEN];
	char eq[5][MAX_INPUT_LEN / 5];
	char *ptr;

	int lead, mid, cons, mu, j, gcd1;
	int i = 0, found = 0;

	/* actually doing stuff */
	dieon(argc != 2, "need 1 argument\n");
	dieon(strlen(argv[1]) > MAX_INPUT_LEN, "argument too long\n");

	strlcpy(input, argv[1], MAX_INPUT_LEN);
	ptr = sstrtok(input, " ");

	while (ptr != NULL) {
		strlcpy(eq[i], ptr, MAX_INPUT_LEN / 5);
		ptr = strtok(NULL, " ");
		i++;
	}
	
	lead = strtoint(eq[0][0], sstrtok(&eq[0][eq[0][0] == '-'], "x"));
	mid = strtoint(eq[1][0], sstrtok(eq[2], "x"));
	cons = strtoint(eq[3][0], eq[4]);

	mu = ABS(lead * cons);
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

	gcd1 = ABS(gcd(lead, i));
	printf("(%dx %c %d)(%dx %c %d)\n", lead / gcd1, SIGN(i), ABS(i / gcd1), gcd1, SIGN((eq[0][0] == '-') ? lead : j), ABS(gcd(j, cons)));
	return EXIT_SUCCESS;
}
