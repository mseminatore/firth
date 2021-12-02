#include <assert.h>
#include <ctype.h>

#include "vm.h"

//
// Create the default environment
//
VM::VM()
{
	interpreter = true;

	create_word("+", OP_PLUS);
	create_word("-", OP_MINUS);
	create_word("*", OP_MUL);
	create_word("/", OP_DIV);

	create_word("dup", OP_DUP);
	create_word("swap", OP_SWAP);
	create_word("drop", OP_DROP);

	create_word(".", OP_PRINT);
	create_word("CR", OP_CR);

	// words
	create_word("func", OP_FUNC);
	create_word(";", OP_ENDWORD);
}

//
//
//
int VM::parse_token(const std::string &token)
{
	if (interpreter)
	{
		// if the word is in the dictionary do it
		if (!exec_word(token))
		{
			// if the word is not in dictionary assume it is a number and push it on the stack
			if (isdigit(token[0]))
			{
				Number num = atoi(token.c_str());

				push_number(num);
//					fprintf(stdout, "%d ", num);
			}
			else
			{
				fprintf(stdout, "%s ?\n", token.c_str());
				return FALSE;
			}
		}
	}
	else
	{
		// TODO - compile a new word
	}

	return TRUE;
}

//
// Create a new word
//
int VM::create_word(const std::string &word, int op)
{
	Word w;
	w.address = bytecode.size();

	bytecode.push_back(op);
	bytecode.push_back(OP_RET);

	auto result = dict.insert(std::pair<const std::string, Word>(word, w));
	if (result.second == false)
		return FALSE;

	return TRUE;
}

//
// Look word up in the dictionary
//
int VM::lookup_word(const std::string &word)
{
	return FALSE;
}

//
// If word is in dictionary execute its code ELSE error
//
int VM::exec_word(const std::string &word)
{
	auto result = dict.find(word);

	if (result == dict.end())
		return FALSE;

	// exec the word
	Word w = result->second;

	return_stack.push(ip);
	ip = w.address;
	
	while(1)
	{
		switch (bytecode[ip])
		{
		// ( n1 n2 -- n1)
		case OP_PLUS:
		{
			auto b = stack.top(); stack.pop();
			auto a = stack.top(); stack.pop();
			stack.push(a + b);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MINUS:
		{
			auto b = stack.top(); stack.pop();
			auto a = stack.top(); stack.pop();
			stack.push(a - b);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MUL:
		{
			auto b = stack.top(); stack.pop();
			auto a = stack.top(); stack.pop();
			stack.push(a * b);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_DIV:
		{
			auto b = stack.top(); stack.pop();
			auto a = stack.top(); stack.pop();
			stack.push(a / b);
		}
			break;

		// ( n1 -- )
		case OP_PRINT:
		{
			auto a = stack.top();
			fprintf(stdout, "%d", a);
			stack.pop();
		}
			break;

		// ( n1 -- n1 n1)
		case OP_DUP:
		{
			auto a = stack.top();
			stack.push(a);
		}
			break;

		// ( n1 -- )
		case OP_DROP:
		{
			stack.pop();
		}
			break;

		// ( n1 n2 -- n2 n1)
		case OP_SWAP:
		{
			auto n1 = stack.top(); stack.pop();
			auto n2 = stack.top(); stack.pop();
			stack.push(n1);
			stack.push(n2);
		}
			break;

		// over ( n1 n2 -- n1 n2 n1 )
		case OP_OVER:
		{
			assert(false);
		}
			break;

		// rot(n1 n2 n3 -- n2 n3 n1)
		case OP_ROT:
		{
			assert(false);
		}
			break;

		case OP_FUNC:
		{
			interpreter = false;
			// TODO - create a new word
//			Word f();
//			create_word(word, f);

			// TODO - parse its code until we see ';'
		}
			break;

		case OP_RET:
		{
			ip = return_stack.top(); return_stack.pop();
			return TRUE;
		}
		break;

		case OP_CR:
		{
			fputs("\n", stdout);
		}
			break;

		default:
			break;
		}
	
		ip++;
	}

	return TRUE;
}
