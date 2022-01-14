#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "vm.h"

static FILE *fin = stdin;
static FILE *fout = stdout;

bool g_bVerbose = false;

//
// get options from the command line
//
int getopt(int n, char *args[])
{
	int i;
	for (i = 1; args[i] && args[i][0] == '-'; i++)
	{
		if (args[i][1] == 'v')
			g_bVerbose = true;
	}

	return i;
}

//
//
//
int main(int argc, char **argv)
{
	VM vm;

	vm.setInputFile(fin);
	vm.setOutputFile(fout);

	while (vm.parse());

	exit(0);
}