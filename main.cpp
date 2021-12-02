#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "vm.h"

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
		auto result = fgets(buf, sizeof(buf), stdin);
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
			fprintf(stdout, " ok\n");
	}

	fputs("done", stdout);
	exit(0);
}