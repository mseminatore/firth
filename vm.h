#pragma once

#include <map>
#include <stack>
#include <vector>

//
typedef int Number;
typedef int Instruction;

const int TRUE = -1;
const int FALSE = 0;

enum
{
	OP_NOP,
	OP_PRINT,
	OP_EMIT,
	OP_CR,
	OP_DOTS,

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

	// logic ops
	OP_AND,
	OP_OR,
	OP_NOT,

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
	OP_DIV
};

//
//
//
struct Word
{
protected:

public:
	int address;
	bool valid;
	
	Word() { address = 0; valid = false; }
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
	std::stack<Number> stack;
	
	// is the VM compiling or interpreting?
	bool interpreter;

	// the word being compiled
	bool wordNamed;

	std::vector<int> bytecode;
	std::stack<int> return_stack;
	int ip;

public:
	VM();
	virtual ~VM() {}

	int parse_token(const std::string &token);
	int lookup_word(const std::string &word, Word &w);
	int exec_word(const std::string &word);
	int create_word(const std::string &word, int op);

	void push_number(const Number &val)
	{
		stack.push(val);
	}

	Number pop_number()
	{
		Number num = stack.top();
		stack.pop();
		// TODO - check for stack underflow
	}
};
