#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "env.h"

//
//
//
int main(int argc, char **argv)
{
	Environment env;
	bool done = false;
	char buf[256];

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
			// if the word is in the dictionary do it
			if (!env.exec_word(token))
			{
				// if the word is not in dictionary assume it is a number and push it on the stack
				if (isdigit(token[0]))
				{
					Number num = atoi(token);

					env.push_number(num);
//					fprintf(stdout, "%d ", num);
				}
				else
				{
					fprintf(stdout, "%s ?\n", token);
					break;
				}
			}

			// get the next token
			token = strtok(NULL, " \n");
		}

		fprintf(stdout, " ok\n");
	}

	fputs("done", stdout);
	exit(0);
}