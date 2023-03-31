#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#include "colour.h"
#include "config.h"

struct dist {
	char *col1, *col2, *col3, *col4, *col5, *col6, *col7, *col8;
	char *getPkgCount;
};
struct dist info = {
  .col1 = BGRAY  "",
  .col2 = BBLUE  "       /\\      ",
  .col3 = BBLUE  "      /  \\     ",
  .col4 = BBLUE  "     /\\   \\    ",
  .col5 = BBLUE  "    /      \\   ",
  .col6 = INDIGO "   /   ,,   \\  "   BBLUE,
  .col7 = INDIGO "  /   |  |  -\\ "   BBLUE,
  .col8 = INDIGO " /_-''    ''-_\\"   BBLUE,
  .getPkgCount = "pacman -Qq | wc -l"
};
char *username, *osname, *shellname, *pkgCount;
char *krnlver;
long uptimeH, uptimeM;

void lowerCase(char *str)
{
	const int len = strlen(str);
	int i;
	for (i = 0; i < len; i += 1)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
}

void upperCase(char *str)
{
	const int len = strlen(str);
	int i;
	for (i = 0; i < len; i += 1)
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] += 'A' - 'a';
}

char *pipeRead(const char *exec)
{
	FILE *pipe = popen(exec, "r");
	if (pipe == NULL)
		return NULL;
	char *returnVal = malloc(256);
	const int scanf_return = fscanf(pipe, "%[^\n]256s", returnVal);
	pclose(pipe);
	if (scanf_return == EOF) {
		fprintf(stderr, "ERROR: scanf failed!\n");
		exit(EXIT_FAILURE);
	}
	return returnVal;
}

void *kernel()
{
	static struct utsname kernelData;
	uname(&kernelData);
	krnlver = kernelData.release;
	return NULL;
}

void *uptime()
{
	struct timespec time;
#if defined(CLOCK_BOOTTIME)
	#define CLOCK CLOCK_BOOTTIME
#elif defined(CLOCK_UPTIME)
	#define CLOCK CLOCK_UPTIME
#elif defined(__APPLE__)
	#define CLOCK CLOCK_MONOTONIC
#endif

#ifdef CLOCK
	clock_gettime(CLOCK, &time);
	uptimeH = time.tv_sec / 3600;
	uptimeM = (time.tv_sec / 60) - (time.tv_sec / 3600 * 60);
#endif
	return NULL;
}

void *user()
{
	username = getenv("USER");
	return NULL;
}

void *shell()
{
	char *shell = getenv("SHELL");
	char *slash = strrchr(shell, '/');
	if (slash) {
		shell = slash + 1;
	}
	shellname = shell;
	return NULL;
}

void *os()
{
  osname = "Archy";
	pkgCount = pipeRead(info.getPkgCount);

	if (ForceLowerCase)
		lowerCase(osname);
	if (ForceUpperCase)
		upperCase(osname);
	return NULL;
}

void *colourDraw()
{
	if (PrintColours == false)
		return NULL;

	printf("                ");
	for (int i = 30; i < 38; i++) {
		printf("\033[0;%dm %s", i, ColourCharacter);
	} // print regular term colours
	printf("\n");
	return NULL;
}

int main()
{
	struct utsname sysInfo;
	uname(&sysInfo);
	pthread_t threads[6];

	pthread_create(&threads[0], NULL, user, NULL);
	pthread_create(&threads[1], NULL, os, NULL);
	pthread_create(&threads[2], NULL, kernel, NULL);
	pthread_create(&threads[3], NULL, uptime, NULL);
	pthread_create(&threads[4], NULL, shell, NULL);

	pthread_join(threads[0], NULL);
	/* os function must be run to get info.col1 */
	pthread_join(threads[1], NULL);
	printf("%s", info.col1);
	printf("%s    %s%s%s\n", info.col2, UserText, TextColour, username);
	printf("%s    %s%s%s\n", info.col3, OsText, TextColour, osname);
	pthread_join(threads[2], NULL);
	printf("%s    %s%s%s\n", info.col4, KernelText, TextColour, krnlver);
	pthread_join(threads[3], NULL);
	printf("%s    %s%s%ldh %ldm\n", info.col5, UptimeText, TextColour, uptimeH,
	       uptimeM);
	pthread_join(threads[4], NULL);
	printf("%s    %s%s%s\n", info.col6, ShellText, TextColour, shellname);
	printf("%s    %s%s%s\n", info.col7, PackageText, TextColour, pkgCount);
	printf("%s\n", info.col8);

	pthread_create(&threads[5], NULL, colourDraw, NULL);
	pthread_join(threads[5], NULL);
	printf("%s", RESET);
	return 0;
}
