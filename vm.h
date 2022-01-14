#pragma once

#include <map>
#include <stack>
#include <vector>

//
typedef int Number;
typedef int Instruction;

const int TRUE = -1;
const int FALSE = 0;
const int UNDEFINED = 0xCCCCCCCC;

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
	OP_DEPTH,

	// variables
	OP_VAR,
	OP_VAR_IMPL,
	OP_CONST,
	OP_FETCH,
	OP_STORE,
	OP_TO_R,
	OP_FROM_R,
	OP_RFETCH,
	OP_LOAD,
	OP_ALLOT,

	// internal compiler opcodes
	OP_FUNC,
	OP_CALL,
	OP_RET,
	OP_LIT,
	OP_DONE,
	OP_GOTO,
	OP_BZ,
	OP_HERE,

	// conditionals and loops
	OP_IF,
	OP_THEN,
	OP_ELSE,
	OP_BEGIN,
	OP_UNTIL,
	OP_DO,
	OP_LOOP,
	OP_WHILE,
	OP_REPEAT,
	OP_EXIT,
	OP_AGAIN,

	// relational ops
	OP_LT,
	OP_GT,
	OP_EQ,
	OP_NE,
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
	
	OP_HALT,

	OP_LAST

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
	int code_addr;		// address of bytecode for this word
	int data_addr;		// for VAR addr of value, for const the value
	bool compileOnly;	// can be executed only in compile mode
	bool immediate;		// execute immediately
	int type;
	int opcode;

	Word() { code_addr = data_addr = opcode = 0; compileOnly = immediate = false; type = OP_FUNC; }
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
	bool compiling;

	// compiled dictionary code
	std::vector<int> bytecode;

	// data segment
	std::vector<int> dataseg;

	// stack of return addrs
	std::stack<int> return_stack;

	int ip, cp;

	FILE *fin, *fout;
	char lval[256];

public:
	VM();
	virtual ~VM() {}

	void load(const std::string &file);

	void setInputFile(FILE *f) { 
		if (f) 
			fin = f; 
		else 
			fin = stdin; 
	}

	void setOutputFile(FILE *f) 
	{ 
		if (f) 
			fout = f; 
		else 
			fout = stdout; 
	}

	int parse();
	int parse_token(const std::string &token);
	int lookup_word(const std::string &word, Word &w);
	int exec_word(const std::string &word);
	int create_word(const std::string &word, const Word &w);
	int define_word(const std::string &word, int op, bool compileOnly = false);

	int define_word_var(const std::string &word, int val);
	int define_word_var(const std::string &word, int val, int daddr);
	int define_word_const(const std::string &word, int val);

	int interpret(const std::string &token);
	int compile(const std::string &token);
	int compile_time(const Word &w);

	void emit(int op);

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
