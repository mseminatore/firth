#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "firth.h"

static FILE *fin = stdin;
static FILE *fout = stdout;

bool g_bVerbose = false;

Firth *g_pFirth = NULL;

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

// custom word
int isTwo(void)
{
	auto n = g_pFirth->pop();
	g_pFirth->push(n == 2 ? TRUE : FALSE);

	return TRUE;
}

//
//
//
int main(int argc, char **argv)
{
	g_pFirth = new Firth();

	// load core libraries
	g_pFirth->loadCore();

	// add our own custom words
	g_pFirth->define_user_word("istwo", isTwo);

	// add a const and a var
	g_pFirth->define_word_const("PI", 31415);
	g_pFirth->define_word_var("myVar", 16);

	// call Firth from native code
	g_pFirth->push(1);
	g_pFirth->push(2);
	g_pFirth->exec_word("+");
	g_pFirth->exec_word(".");

	g_pFirth->setInputFile(fin);
	g_pFirth->setOutputFile(fout);

	while (g_pFirth->parse());

	return 0;
}