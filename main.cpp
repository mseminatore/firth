#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
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

// custom word functions
static int isEven(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) == 0 ? F_TRUE : F_FALSE);

	return F_TRUE;
}

static int isOdd(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) ? F_TRUE : F_FALSE);

	return F_TRUE;
}

// register our collection of custom words
static const struct FirthRegister myWords[] =
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

FirthNumber tickCount;

//
// This is an example of the Firth REPL embedded in another program.
//
int main(int argc, char **argv)
{
	g_pFirth = new Firth();

	g_pFirth->set_input_file(fin);
	g_pFirth->set_output_func(myprint);

	// load (optional) core libraries
	g_pFirth->load_core();

	// add our own custom words
	g_pFirth->register_words(myWords);

	// add a const and a var
	g_pFirth->define_word_const("APP.VER", 1);
	g_pFirth->define_word_var("System.Tick", &tickCount);

	// examples of calling Firth from native code
	
	// do_word is a set of convenience methods to push 
	// 1, 2, or 3 parameters on stack and execute a word
	g_pFirth->do_word("+", 1, 2);
	
	// execute any defined word, no parameters
	g_pFirth->exec_word(".");

	// REPL loop
	int active = F_TRUE;

	while (active)
	{
		tickCount = GetTickCount();
		active = g_pFirth->parse();
	}

	return 0;
}