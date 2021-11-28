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
	OP_WORD,
	OP_PRINT,
	OP_CR,

	OP_DUP,
	OP_SWAP,
	OP_DROP,
	OP_ROT,
	OP_OVER,

	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV
};

//
//
//
class Function
{
protected:
	typedef std::vector<Instruction> Code;
	Code code;

public:
	Function(Instruction inst)
	{
		code.push_back(inst);
	}

	Code::iterator begin() { return code.begin(); }
	Code::iterator end() { return code.end(); }
};

//
//
//
class Environment
{
protected:
	// dictionary of words
	std::map<std::string, Function> dict;

	// the data stack
	std::stack<Number> stack;

public:
	Environment();
	virtual ~Environment() {}

	int lookup_word(const std::string &word);
	int exec_word(const std::string &word);
	int create_word(const std::string &word, Function &f);

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
