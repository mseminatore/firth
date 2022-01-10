#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <ctype.h>

#include "vm.h"

//
// Create the default environment
//
VM::VM()
{
	cp = 0;
	ip = 0;

	dataseg.push_back(0);

	interpreter = true;
	compiling = true;

	// special opcode that tells us when execution is finished
	emit(OP_DONE);

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

	// control words
	define_word("GOTO", OP_GOTO, true);
	define_word("BZ", OP_BZ, true);
	define_word("IF", OP_IF, true);
	define_word("THEN", OP_THEN, true);
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
	define_word("R@", OP_RFETCH);

	// IO words
	define_word(".", OP_PRINT);
	define_word("emit", OP_EMIT);
	define_word(".S", OP_DOTS);
//	define_word(".\"", OP_DOTQUOTE);

	// variable and constant words
	define_word("var", OP_VAR);
	define_word("__var_impl", OP_VAR_IMPL);
	define_word("const", OP_CONST);
	define_word("@", OP_FETCH);
	define_word("!", OP_STORE);
	define_word("allot", OP_ALLOT);

	// compiler words
	define_word(":", OP_FUNC);
	define_word("func", OP_FUNC);
	define_word("fn", OP_FUNC);
	define_word("def", OP_FUNC);
	define_word("load", OP_LOAD);

	// pre-defined variables
	define_word_var("CP", 0, 0);

	// core words
	FILE *f = fopen("core.fth", "rt");
	setInputFile(f);
	setOutputFile(stdout);
	while (parse());
	fclose(f);
}

//
void VM::emit(int op)
{
	bytecode.push_back(op);

	// update CP
	dataseg[cp] = bytecode.size();
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

	// EOL comments
	if (chr == '\\')
	{
		skipToEOL();
		goto lex01;
	}

	// inline comments
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
	if (token == "]")
	{
		interpreter = false;
		return TRUE;
	}

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
int VM::parse()
{
	int token;
	int err = TRUE;

	do
	{
		if (fin == stdin)
			fputs("\nfirth> ", fout);

		while ((token = lex()) != '\n' && token != EOF)
		{
			err = parse_token(lval);
			if (!err)
			{
				//				assert(false);
			}
		}

		if (token == '\n' && err == TRUE && fin == stdin)
			fputs(" ok", fout);

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

	emit(op);
	emit(OP_RET);

	return TRUE;
}

//
int VM::define_word_var(const std::string &word, int val, int daddr)
{
	Word var_word;

	if (!lookup_word("__var_impl", var_word))
		return FALSE;

	Word w;

	// all vars call the same run-time code
	w.address = var_word.address;
	w.data = daddr;
	w.type = OP_VAR;

	if (FALSE == create_word(word, w))
		return FALSE;

	return TRUE;
}

//
int VM::define_word_var(const std::string &word, int val)
{
	if (FALSE == define_word_var(word, val, dataseg.size()))
		return FALSE;

	dataseg.push_back(val);
	return TRUE;
}

//
int VM::define_word_const(const std::string &word, int val)
{
	Word w;

	// all vars call the same run-time code
	w.address = bytecode.size();
	w.data = val;
	w.type = OP_CONST;

	emit(OP_LIT);
	emit(val);
	emit(OP_RET);

	if (FALSE == create_word(word, w))
		return FALSE;

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
int VM::compile(const std::string &token)
{
	if (token == "[")
	{
		interpreter = true;
		return TRUE;
	}

	// look for end of current word
	if (!strcmp(token.c_str(), ";"))
	{
		emit(OP_RET);
		interpreter = true;
		return TRUE;
	}

	// compile the call to the existing WORD
	Word w;
	if (lookup_word(token, w))
	{
		emit(OP_CALL);
		emit(w.address);
	}
	else
	{
		// or compile the NUMBER
		if (isdigit(token[0]) || token[0] == '-')
		{
			Number num = atoi(token.c_str());
			emit(OP_LIT);
			emit(num);
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
//int VM::compile_time()
//{
//}

//
// If word is in the dictionary execute its code, ELSE error
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
			push(n1 + n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MINUS:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 - n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MUL:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 * n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_DIV:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 / n2);
		}
			break;

		// ( n1 n2 -- rem)
		case OP_MOD:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 % n2);
		}
			break;

		// ( n1 n2 -- rem quotient)
		case OP_DIVMOD:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 % n2); // push rem
			push(n1 / n2); // push quotient
		}
			break;

		// ( n1 n2 n3 -- n4)
		case OP_MULDIV:
		{
			auto n3 = stack.top(); stack.pop();
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			long long n4 = long long(n1) * n2;
			push(int(n4/n3));
		}
			break;

		// ( n1 n2 -- n1)
		case OP_POW:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(int(pow(n1, n2) + 0.5f));
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
			push(a);
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
			push(n2);
			push(n1);
		}
			break;

		// over ( n1 n2 -- n1 n2 n1 )
		case OP_OVER:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1);
			push(n2);
			push(n1);
		}
			break;

		// rot (n1 n2 n3 -- n2 n3 n1)
		case OP_ROT:
		{
			auto n3 = stack.top(); stack.pop();
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n2);
			push(n3);
			push(n1);
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
			push(num);
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
			push(n1 < n2 ? TRUE : FALSE);
		}
			break;

		// greater than (n1 n2 -- bool)
		case OP_GT:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 > n2 ? TRUE : FALSE);
		}
			break;

		// 0= ( n -- f)
		case OP_ZEQ:
		{
			auto n = stack.top(); stack.pop();
			push(n == 0 ? TRUE : FALSE);
		}
			break;

		// 0< ( n -- f)
		case OP_ZLT:
		{
			auto n = stack.top(); stack.pop();
			push(n < 0 ? TRUE : FALSE);
		}
			break;

		// 0> ( n -- f)
		case OP_ZGT:
		{
			auto n = stack.top(); stack.pop();
			push(n > 0 ? TRUE : FALSE);
		}
			break;

		// 0<> (n -- f)
		case OP_ZNE:
		{
			auto n = stack.top(); stack.pop();
			push(n != 0 ? TRUE : FALSE);
		}
			break;

		// and (n1 n1 -- n1)
		case OP_AND:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 & n2);
		}
			break;

		// or (n1 n2 -- n1)
		case OP_OR:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 | n2);
		}
			break;

		// not (n1 -- ~n1)
		case OP_NOT:
		{
			auto n1 = stack.top(); stack.pop();
			push(!n1);
		}
			break;

		// xor (n1 n2 -- n1)
		case OP_XOR:
		{
			auto n2 = stack.top(); stack.pop();
			auto n1 = stack.top(); stack.pop();
			push(n1 ^ n2);
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
			push(n1 == n2 ? TRUE : FALSE);
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
			push(n);
		}
			break;

		// R@ ( -- n )
		case OP_RFETCH:
		{
			auto n = return_stack.top();
			push(n);
		}
			break;

		// GOTO (addr -- )
		case OP_GOTO:
		{
			auto addr = bytecode[ip];
			ip = addr;
		}
			break;

		// BZ (n -- )
		case OP_BZ:
		{
			auto n = stack.top(); stack.pop();
			if (0 == n)
			{
				auto addr = bytecode[ip];
				ip = addr;
			}
			else
				ip++;	// skip addr
		}
			break;

		// IF (f -- )
		case OP_IF:
		{
			// compile-time behavior
			emit(OP_BZ);
			push(bytecode.size());
			emit(UNDEFINED);

			// run-time behavior
			auto n = stack.top(); stack.pop();
			if (!n)
			{
				// get the address
				auto addr = bytecode[ip];
				ip = addr;
			}
		}
		break;

		// THEN ( -- )
		case OP_THEN:
		{
			// compile-time behavior
			auto dest = stack.top(); stack.pop();
			bytecode[dest] = bytecode.size();
		}
			break;

		// @ (addr -- val)
		case OP_FETCH:
		{
			auto addr = stack.top(); stack.pop();
			auto val = dataseg[addr];
			push(val);
		}
			break;

		// ! (addr val -- )
		case OP_STORE:
		{
			auto addr = stack.top(); stack.pop();
			auto val = stack.top(); stack.pop();
			dataseg[addr] = val;
		}
			break;

		// var ( -- )
		case OP_VAR:
		{
			lex();
			define_word_var(lval, UNDEFINED);
		}
			break;

		// ( -- addr)
		case OP_VAR_IMPL:
		{
			// get addr of the variable and push it onto the stack
			push(w.data);
		}
			break;

		// const ( -- )
		case OP_CONST:
		{
			auto val = stack.top(); stack.pop();

			lex();
			define_word_const(lval, val);
		}
			break;

		// allot ( n -- )
		case OP_ALLOT:
		{
			auto count = stack.top(); stack.pop();
			
			while (count--)
				dataseg.push_back(UNDEFINED);
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	return TRUE;
}
