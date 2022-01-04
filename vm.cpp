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
	define_word("+", OP_PLUS);
	define_word("-", OP_MINUS);
	define_word("*", OP_MUL);
	define_word("/", OP_DIV);
	define_word("mod", OP_MOD);
	define_word("/mod", OP_DIVMOD);
	define_word("*/", OP_MULDIV);
	define_word("pow", OP_POW);

	// relational words
	define_word("<", OP_LT);
	define_word(">", OP_GT);
	define_word("=", OP_EQ);
	define_word("0=", OP_ZEQ);
	define_word("0<", OP_ZLT);
	define_word("0>", OP_ZGT);
	define_word("0<>", OP_ZNE);

	define_word("IF", OP_IF, true);
	define_word("THEN", OP_THEN, true);
	define_word("ELSE", OP_ELSE, true);
	define_word("ELSE", OP_ELSE, true);

	// logic words
	define_word("and", OP_AND);
	define_word("or", OP_OR);
	define_word("not", OP_NOT);
	define_word("xor", OP_XOR);

	// stack words
	define_word("dup", OP_DUP);
	define_word("swap", OP_SWAP);
	define_word("drop", OP_DROP);
	define_word("rot", OP_ROT);
	define_word("over", OP_OVER);
	define_word(">R", OP_TO_R);
	define_word("R>", OP_FROM_R);

	// IO words
	define_word(".", OP_PRINT);
	define_word("emit", OP_EMIT);
	define_word(".S", OP_DOTS);
//	define_word(".\"", OP_DOTQUOTE);

	// variable and constant words
	define_word("var", OP_VAR);
	define_word("const", OP_CONST);
	define_word("@", OP_VLOAD);
	define_word("!", OP_STORE);

	// compiler words
	define_word(":", OP_FUNC);
	define_word("func", OP_FUNC);
	define_word("fn", OP_FUNC);
	define_word("def", OP_FUNC);
	define_word("load", OP_LOAD);

	// core words
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
//
//
void VM::skipToEOL(void)
{
	int c;

	// skip to EOL
	do {
		c = getChar();
	} while (c != '\n' && c != EOF);

	// put last character back
	ungetChar(c);
}

//
//
//
void VM::skipToChar(int chr)
{
	int c;

	// skip to char
	do {
		c = getChar();
	} while (c != chr && c != EOF);
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

	// check for comments
	if (chr == '\\')
	{
		skipToEOL();
		goto lex01;
	}

	if (chr == '(')
	{
		skipToChar(')');
		goto lex01;
	}

	if (chr == EOF || chr == '\n')
		return chr;

	// look for a word
	do
	{
		*pBuf++ = chr;
	} while ((chr = getChar()) != EOF && chr != ' ' && chr != '\n');

	ungetChar(chr);

	*pBuf = 0;
	return TOK_WORD;
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
		if (fin == stdin)
			fputs("\nfirth> ", fout);

		while((token = lex()) != '\n' && token != EOF)
		{
			err = parse_token(lval);
			if (!err)
			{
//				assert(false);
			}
		}
	} while (token != EOF);

	if (fin != stdin)
	{
		fclose(fin);
		setInputFile(stdin);
	}

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
int VM::interpret(const std::string &token)
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

	return TRUE;
}

//
int VM::compile(const std::string &token)
{
	// look for end of current word
	if (!strcmp(token.c_str(), ";"))
	{
		bytecode.push_back(OP_RET);
		interpreter = true;
		return TRUE;
	}

	// compile the call to the existing WORD
	Word w;
	if (lookup_word(token, w))
	{
		bytecode.push_back(OP_CALL);
		bytecode.push_back(w.address);
	}
	else
	{
		// or compile the NUMBER
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

	return TRUE;
}

//
//
//
int VM::parse_token(const std::string &token)
{
	if (interpreter)
		return interpret(token);

	return compile(token);
}

//
//
//
int VM::create_word(const std::string &word, const Word &w)
{
	// TODO - we might want to succeed in overwriting existing words here
	auto result = dict.insert(std::pair<const std::string, Word>(word, w));
	if (result.second == false)
		return FALSE;

	return TRUE;
}

//
// Create a new builtin word
//
int VM::define_word(const std::string &word, int op, bool compileOnly)
{
	Word w;
	w.address = bytecode.size();
	w.compileOnly = compileOnly;

	if (FALSE == create_word(word, w))
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

	// if WORD not found, error
	if (result == dict.end())
		return FALSE;

	// exec the word
	Word w = result->second;
	if (w.compileOnly)
	{
		fputs(" action is not a function\n", fout);
		return FALSE;
	}

	// handle variables
	if (w.type == OP_VAR)
	{
		// get addr of the variable and put it on the stack
		auto num = w.address;
		stack.push(num);
		return TRUE;
	}

	// handle constants
	if (w.type == OP_CONST)
	{
		stack.push(w.address);
		return TRUE;
	}

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

			lex();
			Word w;
			w.address = bytecode.size();
			w.type = OP_FUNC;
			create_word(lval, w);

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
			auto n = stack.top(); stack.pop();
			stack.push(n == 0 ? TRUE : FALSE);
		}
			break;

		// 0< ( n -- f)
		case OP_ZLT:
		{
			auto n = stack.top(); stack.pop();
			stack.push(n < 0 ? TRUE : FALSE);
		}
			break;

		// 0> ( n -- f)
		case OP_ZGT:
		{
			auto n = stack.top(); stack.pop();
			stack.push(n > 0 ? TRUE : FALSE);
		}
			break;

		// 0<> (n -- f)
		case OP_ZNE:
		{
			auto n = stack.top(); stack.pop();
			stack.push(n != 0 ? TRUE : FALSE);
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

		// IF (f -- )
		case OP_IF:
		{
			auto n = stack.top(); stack.pop();
			if (!n)
			{
				// get the address
				auto addr = bytecode[ip];
				ip = addr;
			}
		}
		break;

		// @ (addr -- val)
		case OP_VLOAD:
		{
			auto addr = stack.top(); stack.pop();
			auto val = bytecode[addr];
			stack.push(val);
		}
			break;

		// ! (addr val -- )
		case OP_STORE:
		{
			auto addr = stack.top(); stack.pop();
			auto val = stack.top(); stack.pop();
			bytecode[addr] = val;
		}
			break;

		// var ( -- )
		case OP_VAR:
		{
			lex();
			Word w;
			w.address = bytecode.size();
			w.type = OP_VAR;

			// 0xDEAD is an uninitialized variable
			bytecode.push_back(0xDEAD);

			create_word(lval, w);
		}
			break;

		// const ( -- )
		case OP_CONST:
		{
			auto val = stack.top(); stack.pop();

			lex();
			Word w;
			w.type = OP_CONST;
			w.address = val;

			create_word(lval, w);
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	return TRUE;
}
