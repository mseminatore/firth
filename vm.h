#pragma once

#include <map>
#include <stack>
#include <vector>

//
typedef int Number;
typedef int Instruction;

const int TRUE = -1;
const int FALSE = 0;

extern bool g_bVerbose;

// tokens
enum
{
	TOK_WORD = 256,
	TOK_NUM
};

// opcodes
enum
{
	OP_NOP,
	OP_PRINT,
	OP_EMIT,
	OP_CR,
	OP_DOTS,
	OP_DOTQUOTE,
	OP_VAR,
	OP_CONST,
	OP_LOAD,
	OP_STORE,
	OP_TO_R,
	OP_FROM_R,

	// internal compiler opcodes
	OP_FUNC,
	OP_CALL,
	OP_RET,
	OP_LIT,
	OP_DONE,

	// relational ops
	OP_LT,
	OP_GT,
	OP_EQ,
	OP_ZEQ,
	OP_ZLT,
	OP_ZGT,

	// logic ops
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_XOR,

	// stack ops
	OP_DUP,
	OP_SWAP,
	OP_DROP,
	OP_ROT,
	OP_OVER,

	// math ops
	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_DIVMOD,
	OP_MULDIV,
	OP_POW
};

//
//
//
struct Word
{
protected:

public:
	int address;
	bool compileOnly;

	Word() { address = 0; compileOnly = false; }
};

//
//
//
class VM
{
protected:
	// dictionary of words
	std::map<std::string, Word> dict;

	// the data stack
	typedef std::stack<Number> Stack;
	Stack stack;
	
	// is the VM compiling or interpreting?
	bool interpreter;

	// the word being compiled
	bool wordNamed;

	std::vector<int> bytecode;
	std::stack<int> return_stack;
	int ip;
	FILE *fin, *fout;
	char lval[256];

public:
	VM();
	virtual ~VM() {}

	void setInputFile(FILE *f) { if (f) fin = f; else fin = stdin; }
	void setOutputFile(FILE *f) { if (f) fout = f; else fout = stdout; }

	int parse();
	int parse_token(const std::string &token);
	int lookup_word(const std::string &word, Word &w);
	int exec_word(const std::string &word);
	int create_word(const std::string &word, int op);

	void push(const Number &val)
	{
		stack.push(val);
	}

	int pop(Number *pNum);

	// lexical analyzer methods
	int getChar();
	void ungetChar(int c);
	bool isWhitespace(int c);
	bool isNumber(int c);
	bool isWord();
	int skipLeadingWhiteSpace();
	int lex();
};
