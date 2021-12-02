#include <assert.h>
#include <ctype.h>

#include "vm.h"

//
// Create the default environment
//
VM::VM()
{
	interpreter = true;

	// special opcode that tells us when execution is finished
	bytecode.push_back(OP_DONE);

	// math words
	create_word("+", OP_PLUS);
	create_word("-", OP_MINUS);
	create_word("*", OP_MUL);
	create_word("/", OP_DIV);

	// relational words
	create_word("<", OP_LT);
	create_word(">", OP_GT);
	create_word("=", OP_EQ);

	// logic words
	create_word("and", OP_AND);
	create_word("or", OP_OR);
	create_word("not", OP_NOT);

	// stack words
	create_word("dup", OP_DUP);
	create_word("swap", OP_SWAP);
	create_word("drop", OP_DROP);

	// IO words
	create_word(".", OP_PRINT);
	create_word("emit", OP_EMIT);
	create_word("CR", OP_CR);
	create_word(".S", OP_DOTS);

	// compiler words
	create_word("func", OP_FUNC);
	create_word(":", OP_FUNC);
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
		if (!strcmp(token.c_str(), ";"))
		{
			bytecode.push_back(OP_RET);
			interpreter = true;
			wordNamed = false;
			return TRUE;
		}

		// compile a new word
		// get the name
		if (!wordNamed)
		{
			// set start address
			Word w;
			w.address = bytecode.size();

			// TODO - we might want to succeed in overwriting existing words here
			auto result = dict.insert(std::pair<const std::string, Word>(token, w));
			if (result.second == false)
				return FALSE;

			wordNamed = true;
		}
		else
		{
			Word w;
			if (lookup_word(token, w))
			{
				bytecode.push_back(OP_CALL);
				bytecode.push_back(w.address);
			}
			else
			{
				if (isdigit(token[0]))
				{
					Number num = atoi(token.c_str());
					bytecode.push_back(OP_LIT);
					bytecode.push_back(num);
				}
				else
				{
					fprintf(stdout, "%s ?\n", token.c_str());
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

//
// Create a new builtin word
//
int VM::create_word(const std::string &word, int op)
{
	Word w;
	w.address = bytecode.size();

	// TODO - we might want to succeed in overwriting existing words here
	auto result = dict.insert(std::pair<const std::string, Word>(word, w));
	if (result.second == false)
		return FALSE;

	bytecode.push_back(op);
	bytecode.push_back(OP_RET);

	return TRUE;
}

//
// Look word up in the dictionary
//
int VM::lookup_word(const std::string &word, Word &w)
{
	auto result = dict.find(word);

	if (result == dict.end())
		return FALSE;

	w = result->second;
	return TRUE;
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
		switch (bytecode[ip++])
		{
		// execution is complete
		case OP_DONE:
		{
			ip = 0;
			return TRUE;
		}
			break;

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
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n2);
			stack.push(n1);
		}
			break;

		// over ( n1 n2 -- n1 n2 n1 )
		case OP_OVER:
		{
			assert(false);
		}
			break;

		// rot (n1 n2 n3 -- n2 n3 n1)
		case OP_ROT:
		{
			assert(false);
		}
			break;

		case OP_FUNC:
		{
			interpreter = false;
			wordNamed = false;
			return TRUE;
		}
			break;

		case OP_CALL:
		{
			auto addr = bytecode[ip];
			return_stack.push(ip + 1);
			ip = addr;
		}
			break;

		case OP_RET:
		{
			ip = return_stack.top(); return_stack.pop();
		}
			break;

		case OP_LIT:
		{
			auto num = bytecode[ip];
			stack.push(num);
		}
			break;

		// ( -- )
		case OP_CR:
		{
			fputs("\n", stdout);
		}
			break;

		// ( c -- )
		case OP_EMIT:
		{
			auto c = stack.top(); stack.pop();
			fputc(c, stdout);
		}
			break;

		// less than (n1 n2 -- bool)
		case OP_LT:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 < n2 ? TRUE : FALSE);
		}
			break;

		// greater than (n1 n2 -- bool)
		case OP_GT:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 > n2 ? TRUE : FALSE);
		}
			break;

		// and (n1 n1 -- n1)
		case OP_AND:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 & n2 ? TRUE : FALSE);
		}
			break;

		// or (n1 n2 -- n1)
		case OP_OR:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 | n2 ? TRUE : FALSE);
		}
			break;

		// not (n1 -- !n1)
		case OP_NOT:
		{
			auto n1 = stack.top(); stack.pop();
			stack.push(!n1);
		}
			break;

		// .S
		case OP_DOTS:
		{

		}
			break;

		// equal to (n1 n2 -- bool)
		case OP_EQ:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 == n2 ? TRUE : FALSE);
		}
		break;

		default:
			assert(false);
			break;
		}
	}

	return TRUE;
}
