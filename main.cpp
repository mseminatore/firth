#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "vm.h"

FILE *fin = stdin;
FILE *fout = stdout;

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
	bool done = false;
	char buf[256];
	int err;

	while (!done)
	{
		// read a line
		fputs("\nfirth> ", fout);
		auto result = fgets(buf, sizeof(buf), fin);
		if (!result)
		{
			break;
		}

		// parse the line for words (space delimited)
		char *token = strtok(buf, " \n");
		while (token)
		{
			err = vm.parse_token(token);
			if (err)
			{
				// get the next token
				token = strtok(NULL, " \n");
			}
			else
				break;
		}

		if (err)
			fprintf(fout, " ok\n");
	}

	fputs("done", fout);
	exit(0);
}