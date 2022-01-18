#pragma once

#include <map>
#include <stack>
#include <vector>

//
typedef int FirthNumber;
typedef int Instruction;
typedef float FirthFloat;

const int FTH_TRUE = -1;
const int FTH_FALSE = 0;
const int FTH_UNDEFINED = 0xCDCDCDCD;

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
	OP_SPRINT,
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
	OP_NATIVE_FUNC,
	OP_CALL,
	OP_RET,
	OP_LIT,
	OP_DONE,
	OP_GOTO,
	OP_BZ,
	OP_HERE,
	OP_WORDS,

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

	// bitwise and logic ops
	OP_AND,
	OP_OR,
	OP_NOT,
	OP_XOR,
	OP_RSHIFT,
	OP_LSHIFT,

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
};

// type of native words
class Firth;
typedef int(*FirthFunc)(Firth *pFirth);

// type for custom output functions
typedef void(*FirthOutputFunc)(char*);

//
// Structure of a Firth Word
//
struct Word
{
protected:

public:
	int code_addr;				// offset address of bytecode for this word
	FirthNumber *data_addr;		// for VAR addr of value, for const the value
	bool compileOnly;			// can be executed only in compile mode
	bool immediate;				// execute immediately
	int type;					// type of this word
	int opcode;					// bytecode for this word
	bool hidden;				// Word not visible

	FirthFunc nativeWord;

	Word() { 
		code_addr = opcode = 0; 
		data_addr = nullptr;
		compileOnly = immediate = hidden = false; 
		type = OP_FUNC; 
		nativeWord = nullptr;
	}
};

// custom function registration struct
struct FirthWordSet
{
	char *wordName;
	FirthFunc func;
};

//
//
//
class Firth
{
protected:
	struct WordLessThan
	{
		bool operator() (const std::string &a, const std::string &b) const
		{
			return _strcmpi(a.c_str(), b.c_str()) < 0 ;
		}
	};

	// dictionary of words
	std::map<std::string, Word, WordLessThan> dict;

	// reverse lookup for disassembly of bytecodes
	std::map<int, std::string> disasm;

	// the data stack
	typedef std::stack<FirthNumber> Stack;
	Stack stack;

	// is Firth compiling or interpreting?
	bool interpreter;

	// compiled dictionary code
	std::vector<int> bytecode;

	// data segment
	FirthNumber *pDataSegment;
	FirthNumber *DP;
	unsigned data_limit;

	// stack of return addrs
	std::stack<int> return_stack;

	// float stack
	std::stack<FirthFloat> fstack;

	int ip;
	FirthNumber *CP;
	int hexmode;
	bool halted;

	FILE *fin;
	const char *txtInput;
	char lval[256];

	FirthOutputFunc firth_print;

	//
	// non-public methods
	//
	int load(const std::string &file);
	void emit(int op);

	// lexical analyzer methods
	int getChar();
	void ungetChar(int c);
	bool isWhitespace(int c);
	bool isNumber(int c);
	void skipToEOL(void);
	void skipToChar(int c);
	int skipLeadingWhiteSpace();
	int lex(int delim = ' ');
	bool isInteger(const char *s);

	int define_word(const std::string &word, int op, bool compileOnly = false);
	int create_word(const std::string &word, const Word &w);
	int make_hidden(const std::string &word, bool flag);

public:
	Firth(unsigned data_limit = 1024);
	virtual ~Firth() {}

	void set_input_file(FILE *f) 
	{ 
		if (f) 
			fin = f; 
		else 
			fin = stdin; 
	}

	void set_output_func(FirthOutputFunc func) { firth_print = func; }
	void firth_printf(char *format, ...);

	int parse();
	int parse_token(const std::string &token);
	int parse_string(const std::string &line);

	int lookup_word(const std::string &word, Word **ppWord);
	int exec_word(const std::string &word);

	int define_word_var(const std::string &word, FirthNumber val);
	int define_word_var(const std::string &word, FirthNumber *val);

	int define_word_const(const std::string &word, FirthNumber val);
	int define_user_word(const std::string &word, FirthFunc func, bool compileOnly = false);

	int interpret(const std::string &token);
	int compile(const std::string &token);
	int compile_time(const Word &w);

	int load_library(const std::string &file)
	{
		return load(file);
	}

	int load_core()
	{
		// core words
		return load_library("core.fth");
	}

	int register_wordset(const FirthWordSet words[]);

	void push(const FirthNumber &val)
	{
		stack.push(val);
	}

	FirthNumber pop();
	FirthNumber top() { return stack.top(); }

	FirthFloat popf();
	void pushf(const FirthFloat &f) { fstack.push(f); }
	FirthFloat topf() { return fstack.top(); }

	unsigned data_size() { return (unsigned)(DP - pDataSegment); }
	void push_data(FirthNumber number) 
	{
		if (data_size() >= data_limit)
		{
			firth_print("Out of data space!\n");
			return;
		}

		*DP++ = number;
	}
	
	// convenience methods for executing words
	int do_word(const std::string &word, FirthNumber n) { push(n); return exec_word(word); }
	int do_word(const std::string &word, FirthNumber n1, FirthNumber n2) { push(n1); push(n2); return exec_word(word); }
	int do_word(const std::string &word, FirthNumber n1, FirthNumber n2, FirthNumber n3) { push(n1); push(n2); push(n3); return exec_word(word); }
};
