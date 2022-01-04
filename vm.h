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
	OP_DOTS,
	OP_DOTQUOTE,
	OP_VAR,
	OP_CONST,
	OP_VLOAD,
	OP_STORE,
	OP_TO_R,
	OP_FROM_R,
	OP_LOAD,

	// internal compiler opcodes
	OP_FUNC,
	OP_CALL,
	OP_RET,
	OP_LIT,
	OP_DONE,
	OP_GOTO,
	OP_HERE,

	// conditionals
	OP_IF,
	OP_THEN,
	OP_ELSE,

	// relational ops
	OP_LT,
	OP_GT,
	OP_EQ,
	OP_ZEQ,
	OP_ZLT,
	OP_ZGT,
	OP_ZNE,

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
	OP_POW,
	//OP_ABS,
	//OP_MIN,
	//OP_MAX
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
	int type;

	Word() { address = 0; compileOnly = false; type = -1; }
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
	int create_word(const std::string &word, const Word &w);
	int define_word(const std::string &word, int op, bool compileOnly = false);
	int interpret(const std::string &token);
	int compile(const std::string &token);

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
	void skipToEOL(void);
	void skipToChar(int c);
	int skipLeadingWhiteSpace();
	int lex();
};
