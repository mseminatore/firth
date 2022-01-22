#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>

#include "firth.h"
#include "firth_float.h"

//
static FILE *fin = stdin;
static FILE *fout = stdout;

bool g_bVerbose = false;

FirthNumber tickCount;

char *g_szInputfile = NULL;

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

		if (args[i][1] == 'o')
		{
			fout = fopen(args[i + 1], "wt");
			i++;
		}
	}

	return i;
}

// custom word functions
static int isEven(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) == 0 ? FTH_TRUE : FTH_FALSE);

	return FTH_TRUE;
}

static int isOdd(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) ? FTH_TRUE : FTH_FALSE);

	return FTH_TRUE;
}

// register our collection of custom words
static const struct FirthWordSet myWords[] =
{
	{ "even?", isEven },
	{ "odd?", isOdd },
	{ nullptr, nullptr }
};

// custom output function
static void myprint(char *s)
{
	fputs(s, fout);
}

// examples of calling Firth from native code
void callFirth(Firth *pFirth)
{
	// do_word is a set of convenience methods to push 
	// 1, 2, or 3 parameters on stack and execute a word
	pFirth->do_word("+", 1, 2);

	// execute any defined word, no parameters
	pFirth->exec_word(".");

	// parse and execute a line of text
	pFirth->parse_string("CP @ .");
}

//
void banner(Firth *pFirth)
{
	pFirth->firth_printf("Welcome to Firth! Copyright 2022 by Mark Seminatore\n");
	pFirth->firth_printf("See LICENSE file for usage rights and obligations.\n");
	pFirth->firth_printf("Type 'halt' to quit.\n");
}

//
// This is an example of the Firth REPL embedded in another program.
//
int main(int argc, char **argv)
{
	Firth *pFirth = new Firth();

	// parse command line params
	int firstParam = getopt(argc, argv);

	// setup Firth output
	pFirth->set_output_func(myprint);

	banner(pFirth);

	// load (optional) core libraries
	pFirth->load_core();

	// handle file passed on command line
	if (argc > firstParam)
	{
		g_szInputfile = argv[firstParam];
	}

#if FTH_INCLUDE_FLOAT == 1
	// load (optional) floating point libraries
	firth_register_float(pFirth);
#endif

	// add our own custom words
	pFirth->register_wordset(myWords);

	// add a const and a var
	pFirth->define_word_const("APP.VER", 1);
	pFirth->define_word_var("System.Tick", &tickCount);

	// process command line file input
	if (g_szInputfile)
	{
		FILE *f = fopen(g_szInputfile, "rt");
		if (!f)
		{
			pFirth->firth_printf("File (%s) not found!\n", g_szInputfile);
			exit(-1);
		}
		
		pFirth->set_input_file(f);
		while (pFirth->parse());
		fclose(f);
	}

	//
	// REPL loop
	//

	// setup Firth input
	pFirth->set_input_file(stdin);
	int active = FTH_TRUE;
	while (active)
	{
		tickCount++;
		active = pFirth->parse();
	}

	delete pFirth;

	return 0;
}