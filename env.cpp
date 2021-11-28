#include "env.h"

//
// Create the default environment
//
Environment::Environment()
{
	create_word("+", Function(OP_PLUS));
	create_word("-", Function(OP_MINUS));
	create_word("*", Function(OP_MUL));
	create_word("/", Function(OP_DIV));

	create_word("dup", Function(OP_DUP));
	create_word("swap", Function(OP_SWAP));
	create_word("drop", Function(OP_DROP));

	create_word(".", Function(OP_PRINT));
	create_word("CR", Function(OP_CR));
	create_word("func", Function(OP_FUNC));
}

//
// Create a new word
//
int Environment::create_word(const std::string &word, Function &fn)
{
	auto result = dict.insert(std::pair<const std::string, Function>(word, fn));
	if (result.second == false)
		return FALSE;

	return TRUE;
}

//
// Look word up in the dictionary
//
int Environment::lookup_word(const std::string &word)
{
	return FALSE;
}

//
// If word is in dictionary execute its code ELSE error
//
int Environment::exec_word(const std::string &word)
{
	auto result = dict.find(word);

	if (result == dict.end())
		return FALSE;

	// TODO - exec the function
	auto fn = result->second;

	for (auto iter = fn.begin(); iter != fn.end(); iter++)
	{
		switch (*iter)
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
			fprintf(stdout, "%d ", a);
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

		case OP_CR:
		{
			fputs("\n", stdout);
		}
			break;

		default:
			break;
		}
	}

	return TRUE;
}
