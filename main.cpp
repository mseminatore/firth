#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <ctype.h>

#include "firth.h"
#include "firth_float.h"

static FILE *fin = stdin;
static FILE *fout = stdout;

bool g_bVerbose = false;

Firth *g_pFirth = NULL;

FirthNumber tickCount;

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
	g_pFirth->parse_string("CP @ .");
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
	g_pFirth = new Firth();

	g_pFirth->set_input_file(fin);
	g_pFirth->set_output_func(myprint);

	banner(g_pFirth);

	// load (optional) core libraries
	g_pFirth->load_core();

#if FTH_INCLUDE_FLOAT == 1
	// load (optional) floating point libraries
	firth_register_float(g_pFirth);
#endif

	// add our own custom words
	g_pFirth->register_wordset(myWords);

	// add a const and a var
	g_pFirth->define_word_const("APP.VER", 1);
	g_pFirth->define_word_var("System.Tick", &tickCount);

	// REPL loop
	int active = FTH_TRUE;
	while (active)
	{
		tickCount++;
		active = g_pFirth->parse();
	}

	delete g_pFirth;

	return 0;
}