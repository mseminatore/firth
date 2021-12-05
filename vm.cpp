#define _CRT_SECURE_NO_WARNINGS

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
	create_word("mod", OP_MOD);
	create_word("/mod", OP_DIVMOD);
	create_word("*/", OP_MULDIV);
	create_word("pow", OP_POW);

	// relational words
	create_word("<", OP_LT);
	create_word(">", OP_GT);
	create_word("=", OP_EQ);
	create_word("0=", OP_ZEQ);
	create_word("0<", OP_ZLT);
	create_word("0>", OP_ZGT);

	// logic words
	create_word("and", OP_AND);
	create_word("or", OP_OR);
	create_word("not", OP_NOT);
	create_word("xor", OP_XOR);

	// stack words
	create_word("dup", OP_DUP);
	create_word("swap", OP_SWAP);
	create_word("drop", OP_DROP);
	create_word("rot", OP_ROT);
	create_word("over", OP_OVER);

	// IO words
	create_word(".", OP_PRINT);
	create_word("emit", OP_EMIT);
	create_word("CR", OP_CR);
	create_word(".S", OP_DOTS);
//	create_word(".\"", OP_DOTQUOTE);

	// variable words
	//create_word("var", OP_VAR);
	//create_word("const", OP_CONST);
	//create_word("@", OP_LOAD);
	//create_word("!", OP_STORE);

	// compiler words
	create_word(":", OP_FUNC);
	create_word("func", OP_FUNC);
	create_word("fn", OP_FUNC);
	create_word("def", OP_FUNC);
	
	create_word("load", OP_LOAD);
	create_word(">R", OP_TO_R);
	create_word("R>", OP_FROM_R);

	FILE *f = fopen("core.fth", "rt");
	setInputFile(f);
	setOutputFile(stdout);
	while (parse());
	fclose(f);
}

//
int VM::getChar()
{
	return fgetc(fin);
}

//
void VM::ungetChar(int c)
{
	ungetc(c, fin);
}

//
bool VM::isWhitespace(int c)
{
	return (c == ' ' || c == '\t' /*|| c == '\n' || c == '\r'*/); //? true : false;
}

//
bool VM::isNumber(int c)
{
	// TODO - since minus sign is a word this may be incorrect
	return (isdigit(c) || c == '-');
}

// 
bool VM::isWord()
{
	return false;
}

// skip any leading WS
int VM::skipLeadingWhiteSpace()
{
	int chr;

	do
	{
		chr = getChar();
		//if (chr == '\n')
		//{
		//	// TODO - inc line num and count and reset column count
		//}
	} while (isWhitespace(chr));

	return chr;
}

// return the next token
int VM::lex()
{
	int chr;
	char *pBuf = lval;

lex01:

	// skip any leading WS
	chr = skipLeadingWhiteSpace();

	// TODO - check for comments?

	if (chr == EOF || chr == '\n')
		return chr;

	// TODO - look for a word


	do
	{
		*pBuf++ = chr;
	} while ((chr = getChar()) != EOF && chr != ' ' && chr != '\n');

	ungetChar(chr);

	*pBuf = 0;
	return TOK_WORD;

	// TODO - look for a number?
}

//
//
//
int VM::parse()
{
	int token;
	int err;

	do
	{
		fputs("\nfirth> ", fout);
		while((token = lex()) != '\n' && token != EOF)
		{
			err = parse_token(lval);
			if (!err)
			{
//				assert(false);
			}
		}

		if (fin != stdin)
		{
			fclose(fin);
			setInputFile(stdin);
			continue;
		}

	} while (token != EOF);


	return TRUE;
}

//
//
//
int VM::pop(Number *pNum)
{
	// check for stack underflow
	if (stack.size() == 0)
	{
		fprintf(fout, " Stack underflow\n");
		return FALSE;
	}

	*pNum = stack.top();
	stack.pop();
	return TRUE;
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
			if (isdigit(token[0]) || token[0] == '-')
			{
				Number num = atoi(token.c_str());
				push(num);
			}
			else
			{
				fprintf(fout, "%s ?\n", token.c_str());
				return FALSE;
			}
		}
	}
	else
	{
		// look for end of current word
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
				if (isdigit(token[0]) || token[0] == '-')
				{
					Number num = atoi(token.c_str());
					bytecode.push_back(OP_LIT);
					bytecode.push_back(num);
				}
				else
				{
					fprintf(fout, "%s ?\n", token.c_str());
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
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 + n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MINUS:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 - n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MUL:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 * n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_DIV:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 / n2);
		}
			break;

		// ( n1 n2 -- rem)
		case OP_MOD:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 % n2);
		}
			break;

		// ( n1 n2 -- rem quotient)
		case OP_DIVMOD:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 % n2); // push rem
			stack.push(n1 / n2); // push quotient
		}
			break;

		// ( n1 n2 n3 -- n4)
		case OP_MULDIV:
		{
			auto n3 = stack.top(); stack.pop();
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			long long n4 = long long(n1) * n2;
			stack.push(int(n4/n3));
		}
			break;

		// ( n1 n2 -- n1)
		case OP_POW:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(int(pow(n1, n2) + 0.5f));
		}
			break;

		// ( n1 -- )
		case OP_PRINT:
		{
			auto a = stack.top();
			fprintf(fout, "%d", a);
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
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1);
			stack.push(n2);
			stack.push(n1);
		}
			break;

		// rot (n1 n2 n3 -- n2 n3 n1)
		case OP_ROT:
		{
			auto n3 = stack.top(); stack.pop();
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n2);
			stack.push(n3);
			stack.push(n1);
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
			auto num = bytecode[ip++];
			stack.push(num);
		}
			break;

		// ( -- )
		case OP_CR:
		{
			fputs("\n", fout);
		}
			break;

		// ( c -- )
		case OP_EMIT:
		{
			auto c = stack.top(); stack.pop();
			fputc(c, fout);
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

		// 0= ( n -- f)
		case OP_ZEQ:
		{
			auto n = return_stack.top(); return_stack.pop();
			stack.push(n == 0 ? TRUE : FALSE);
		}
		break;

		// 0< ( n -- f)
		case OP_ZLT:
		{
			auto n = return_stack.top(); return_stack.pop();
			stack.push(n < 0 ? TRUE : FALSE);
		}
		break;

		// 0> ( n -- f)
		case OP_ZGT:
		{
			auto n = return_stack.top(); return_stack.pop();
			stack.push(n > 0 ? TRUE : FALSE);
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

		// not (n1 -- ~n1)
		case OP_NOT:
		{
			auto n1 = stack.top(); stack.pop();
			stack.push(!n1);
		}
			break;

		// xor (n1 n2 -- n1)
		case OP_XOR:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			stack.push(n1 ^ n2);
		}
		break;

		// .S ( -- )
		case OP_DOTS:
		{
			// make a copy of the stack
			Stack s = stack;

			fprintf(fout, "Top -> [ ");
			while(s.size())
			{
				fprintf(fout, "%d ", s.top());
				s.pop();
			}
			fputs("]\n", fout);
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

		//
		case OP_LOAD:
		{
			// get the file name word
			lex();

			// open the file
			FILE *f = fopen(lval, "rt");
			// set files
			if (f)
				setInputFile(f);
		}
			break;

		// >R (n -- )
		case OP_TO_R:
		{
			auto n = stack.top(); stack.pop();
			return_stack.push(n);
		}
			break;

		// R> ( -- n)
		case OP_FROM_R:
		{
			auto n = return_stack.top(); return_stack.pop();
			stack.push(n);
		}
		break;

		default:
			assert(false);
			break;
		}
	}

	return TRUE;
}
