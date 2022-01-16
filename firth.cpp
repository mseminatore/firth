#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>

#include "firth.h"

//
// Create the default environment
//
Firth::Firth()
{
	fin = stdin;
	fout = stdout;

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
	define_word("<>", OP_NE);
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
	define_word("BEGIN", OP_BEGIN, true);
	define_word("UNTIL", OP_UNTIL, true);
	define_word("DO", OP_DO, true);
	define_word("LOOP", OP_LOOP, true);
	define_word("AGAIN", OP_AGAIN, true);
	define_word("EXIT", OP_EXIT, true);
	define_word("WHILE", OP_WHILE, true);
	define_word("REPEAT", OP_REPEAT, true);
	define_word("HALT", OP_HALT);
	define_word("I", OP_RFETCH, true);

	// logic words
	define_word("and", OP_AND);
	define_word("or", OP_OR);
	define_word("not", OP_NOT);
	define_word("xor", OP_XOR);
	define_word("LSHIFT", OP_LSHIFT);
	define_word("RSHIFT", OP_RSHIFT);

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
	define_word(".\"", OP_DOTQUOTE);

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
	define_word("include", OP_LOAD);
	define_word("depth", OP_DEPTH);
	define_word("WORDS", OP_WORDS);

	// pre-defined variables
	define_word_var("CP", 0, cp);
}

//
int Firth::load(const std::string &file)
{
	FILE *f = fopen(file.c_str(), "rt");
	if (!f)
		return F_FALSE;

	setInputFile(f);
	
	parse();

	fclose(f);
	
	return F_TRUE;
}

//
void Firth::emit(int op)
{
	bytecode.push_back(op);

	// update CP
	dataseg[cp] = bytecode.size();
}

//
int Firth::getChar()
{
	return fgetc(fin);
}

//
void Firth::ungetChar(int c)
{
	ungetc(c, fin);
}

//
bool Firth::isWhitespace(int c)
{
	return (c == ' ' || c == '\t' /*|| c == '\n' || c == '\r'*/); //? true : false;
}

//
bool Firth::isNumber(int c)
{
	// TODO - since minus sign is a word this may be incorrect
	return (isdigit(c) || c == '-');
}

//
//
//
void Firth::skipToEOL(void)
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
void Firth::skipToChar(int chr)
{
	int c;

	// skip to char
	do {
		c = getChar();
	} while (c != chr && c != EOF);
}

// skip any leading WS
int Firth::skipLeadingWhiteSpace()
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
int Firth::lex(int delim)
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
	} while ((chr = getChar()) != EOF && chr != delim && chr != '\n');

	ungetChar(chr);

	*pBuf = 0;

	return TOK_WORD;
}

//
//
//
FirthNumber Firth::pop()
{
	// check for stack underflow
	if (stack.size() == 0)
	{
		fprintf(fout, " Stack underflow\n");
		return 0;
	}

	auto num = stack.top();
	stack.pop();
	return num;
}

//
int Firth::interpret(const std::string &token)
{
	if (token == "]")
	{
		interpreter = false;
		return F_TRUE;
	}

	// if the word is in the dictionary do it
	if (!exec_word(token))
	{
		// if the word is not in dictionary assume it is a number and push it on the stack
		if (isdigit(token[0]) || token[0] == '-')
		{
			FirthNumber num = atoi(token.c_str());
			push(num);
		}
		else
		{
			fprintf(fout, "%s ?\n", token.c_str());
			return F_FALSE;
		}
	}

	return F_TRUE;
}

//
//
//
int Firth::parse_string(const std::string &line)
{
	// TODO - break string up into words
	// TODO - pass words to parser
	assert(false);
	return F_TRUE;
}

//
//
//
int Firth::parse_token(const std::string &token)
{
	if (interpreter)
		return interpret(token);

	return compile(token);
}

//
//
//
int Firth::parse()
{
	int token;
	int err = F_TRUE;

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

		if (token == '\n' && err == F_TRUE && fin == stdin)
			fputs(" ok\n", fout);

	} while (token != EOF);

	if (fin != stdin)
	{
		fclose(fin);
		setInputFile(stdin);
	}

	return F_TRUE;
}

//
int Firth::register_words(const FirthRegister words[])
{
	for (int i = 0; words[i].wordName && words[i].func; i++)
	{
		if (F_FALSE == define_user_word(words[i].wordName, words[i].func))
			return F_FALSE;
	}

	return F_TRUE;
}

//
//
//
int Firth::create_word(const std::string &word, const Word &w)
{
	// TODO - we might want to succeed in overwriting existing words here
	auto result = dict.insert(std::pair<const std::string, Word>(word, w));
	if (result.second == false)
		return F_FALSE;

	return F_TRUE;
}

//
// Create a new builtin word
//
int Firth::define_word(const std::string &word, int opcode, bool compileOnly)
{
	Word w;
	w.code_addr = bytecode.size();
	w.compileOnly = compileOnly;
	w.opcode = opcode;

	if (F_FALSE == create_word(word, w))
		return F_FALSE;

	// run-time behavior
	emit(opcode);
	emit(OP_RET);

	// TODO - add opcode/word to diasm
	disasm.insert(std::pair<int, std::string>(opcode, word));

	return F_TRUE;
}

//
// Create a new 
//
int Firth::define_user_word(const std::string &word, FirthFunc func, bool compileOnly)
{
	Word w;

	if (!func)
		return F_FALSE;

	w.nativeWord = func;
	w.compileOnly = compileOnly;

	if (F_FALSE == create_word(word, w))
		return F_FALSE;

	return F_TRUE;
}

//
int Firth::define_word_var(const std::string &word, FirthNumber val, int daddr)
{
	Word var_word;

	if (!lookup_word("__var_impl", var_word))
		return F_FALSE;

	Word w;

	// all vars call the same run-time code
	w.code_addr = var_word.code_addr;
	w.data_addr = daddr;
	w.type = OP_VAR;
	w.opcode = OP_VAR;

	if (F_FALSE == create_word(word, w))
		return F_FALSE;

	return F_TRUE;
}

//
int Firth::define_word_var(const std::string &word, FirthNumber val)
{
	if (F_FALSE == define_word_var(word, val, dataseg.size()))
		return F_FALSE;

	dataseg.push_back(val);
	return F_TRUE;
}

//
int Firth::define_word_var(const std::string &word, FirthNumber *pValue)
{
	Word w;

	// all vars call the same run-time code
	w.code_addr = bytecode.size();
	w.type = OP_USER_VAR;
	w.pUserNumber = pValue;

	emit(OP_USER_VAR);
	emit(OP_RET);

	if (F_FALSE == create_word(word, w))
		return F_FALSE;

	return F_TRUE;
}

//
int Firth::define_word_const(const std::string &word, FirthNumber val)
{
	Word w;

	// all vars call the same run-time code
	w.code_addr = bytecode.size();
	w.data_addr = val;
	w.type = OP_CONST;

	emit(OP_LIT);
	emit(val);
	emit(OP_RET);

	if (F_FALSE == create_word(word, w))
		return F_FALSE;

	return F_TRUE;
}

//
// Look word up in the dictionary
//
int Firth::lookup_word(const std::string &word, Word &w)
{
	auto result = dict.find(word);

	if (result == dict.end())
		return F_FALSE;

	w = result->second;
	return F_TRUE;
}

//
int Firth::compile(const std::string &token)
{
	if (token == "[")
	{
		interpreter = true;
		return F_TRUE;
	}

	// look for end of current word
	if (!strcmp(token.c_str(), ";"))
	{
		emit(OP_RET);
		interpreter = true;
		return F_TRUE;
	}

	// compile the call to the existing WORD
	Word w;
	if (lookup_word(token, w))
	{
		return compile_time(w);
	}
	else
	{
		// or compile the NUMBER
		if (isdigit(token[0]) || token[0] == '-')
		{
			FirthNumber num = atoi(token.c_str());
			emit(OP_LIT);
			emit(num);
		}
		else
		{
			fprintf(fout, "%s ?\n", token.c_str());
			return F_FALSE;
		}
	}

	return F_TRUE;
}

//
int Firth::compile_time(const Word &w)
{
	switch (w.opcode)
	{
	case OP_VAR:
	{
		emit(OP_LIT);
		emit(w.data_addr);
	}
		break;

	case OP_IF:
	{
		// compile-time behavior
		emit(OP_BZ);			// emit conditional branch on zero
		push(bytecode.size());	// push current code pointer onto the stack
		emit(UNDEFINED);		// reserve space for branch address
	}
		break;

	case OP_THEN:
	{
		// compile-time behavior
		auto dest = pop();	// get TOS addr for branch target
		bytecode[dest] = bytecode.size();		// fixup branch target
	}
		break;

	case OP_ELSE:
	{
		// compile-time behavior
		auto dest = pop();	// get TOS addr for branch target

		// setup branch around ELSE clause to the THEN clause
		emit(OP_GOTO);							// unconditional branch
		push(bytecode.size());					// push current code pointer onto the stack for THEN
		emit(UNDEFINED);						// default to next instruction

		// patch IF branch to here, beyond the ELSE
		bytecode[dest] = bytecode.size();		// fixup branch target
	}
		break;

	case OP_BEGIN:
	{
		// compile-time behavior
		push(bytecode.size());	// push current code pointer onto the stack
	}
		break;

	case OP_UNTIL:
	{
		// compile-time behavior
		emit(OP_BZ);
		auto dest = pop();
		emit(dest);
	}
		break;

	case OP_AGAIN:
	{
		// compile-time behavior
		emit(OP_GOTO);
		auto dest = pop();
		emit(dest);
	}
		break;

	case OP_EXIT:
	{
		// TODO - drop/pop N items off stack?? Where N is diff between stack at start of BEGIN
		emit(OP_RET);
	}
		break;

	case OP_WHILE:
	{
		// compile-time behavior
		emit(OP_BZ);			// if test fails jump to after REPEAT
		push(bytecode.size());	// push fixup addr onto stack
		emit(UNDEFINED);		// placeholder for forward address
	}
		break;

	case OP_REPEAT:
	{
		auto while_addr = pop();

		// compile-time behavior
		emit(OP_GOTO);
		auto dest = pop();
		emit(dest);

		bytecode[while_addr] = bytecode.size();	// fixup conditional branch target for WHILE
	}
		break;

	case OP_DO:
	{
		// compile-time behavior
		push(bytecode.size());	// push current code pointer onto the stack
		emit(OP_SWAP);			// save limit and index on return stack
		emit(OP_TO_R);
		emit(OP_TO_R);
	}
		break;
	
	case OP_LOOP:
	{
		// compile-time behavior
		emit(OP_FROM_R);	// get index from return stack
		emit(OP_FROM_R);	// get limit from return stack
		emit(OP_SWAP);
		emit(OP_LIT);
		emit(1);
		emit(OP_PLUS);		// inc index

		emit(OP_OVER);		// make copy of limit index
		emit(OP_OVER);		// one to test, other goes back on return stack

		emit(OP_EQ);		// we consume one copy with test
		emit(OP_BZ);		// conditional branch back to DO

		auto dest = pop();	// get TOS addr for branch target
		emit(dest);
		emit(OP_DROP);		// drop limit index from stack
		emit(OP_DROP);
	}
		break;

	case OP_DOTQUOTE:
	{
		lex('"');
		int c = getChar();	// throw away trailing end-quote
		if (c != '"')
			ungetChar(c);

		//w.data_addr = (int)strdup(lval);
		emit(OP_SPRINT);
		emit((int)_strdup(lval));
	}
		break;

	// user-defined WORDS
	case OP_NOP:
	{
		// compile-time behavior
		emit(OP_CALL);
		emit(w.code_addr);
	}
		break;

	case OP_DONE:
	default:
		emit(w.opcode);
	}

	return F_TRUE;
}

//
// If word is in the dictionary execute its code, ELSE error
//
int Firth::exec_word(const std::string &word)
{
	auto result = dict.find(word);

	// if WORD not found, error
	if (result == dict.end())
		return F_FALSE;

	// exec the word
	Word w = result->second;
	if (w.compileOnly)
	{
		fputs(" action is not a function\n", fout);
		return F_FALSE;
	}

	// execute native words
	if (w.nativeWord)
	{
		return w.nativeWord(this);
	}

	return_stack.push(ip);
	ip = w.code_addr;
	
	while(1)
	{
		switch (bytecode[ip++])
		{
		// execution is complete
		case OP_DONE:
		{
			ip = 0;
			return F_TRUE;
		}
			break;

		// ( n1 n2 -- n1)
		case OP_PLUS:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 + n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MINUS:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 - n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_MUL:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 * n2);
		}
			break;

		// ( n1 n2 -- n1)
		case OP_DIV:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 / n2);
		}
			break;

		// ( n1 n2 -- rem)
		case OP_MOD:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 % n2);
		}
			break;

		// ( n1 n2 -- rem quotient)
		case OP_DIVMOD:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 % n2); // push rem
			push(n1 / n2); // push quotient
		}
			break;

		// ( n1 n2 n3 -- n4)
		case OP_MULDIV:
		{
			auto n3 = pop();
			auto n2 = pop();
			auto n1 = pop();
			long long n4 = long long(n1) * n2;
			push(int(n4/n3));
		}
			break;

		// ( n1 n2 -- n1)
		case OP_POW:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(int(pow(n1, n2) + 0.5f));
		}
			break;

		// ( n1 -- )
		case OP_PRINT:
		{
			auto a = pop();
			fprintf(fout, "%d", a);
		}
			break;

		// ( n1 -- n1 n1)
		case OP_DUP:
		{
			auto a = top();
			push(a);
		}
			break;

		// ( n1 -- )
		case OP_DROP:
		{
			pop();
		}
			break;

		// ( n1 n2 -- n2 n1)
		case OP_SWAP:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n2);
			push(n1);
		}
			break;

		// over ( n1 n2 -- n1 n2 n1 )
		case OP_OVER:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1);
			push(n2);
			push(n1);
		}
			break;

		// rot (n1 n2 n3 -- n2 n3 n1)
		case OP_ROT:
		{
			auto n3 = pop();
			auto n2 = pop();
			auto n1 = pop();
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
			w.code_addr = bytecode.size();
			w.type = OP_FUNC;
			create_word(lval, w);

			return F_TRUE;
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
			auto c = pop();
			fputc(c, fout);
		}
			break;

		// less than (n1 n2 -- bool)
		case OP_LT:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 < n2 ? F_TRUE : F_FALSE);
		}
			break;

		// greater than (n1 n2 -- bool)
		case OP_GT:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 > n2 ? F_TRUE : F_FALSE);
		}
			break;

		// 0= ( n -- f)
		case OP_ZEQ:
		{
			auto n = pop();
			push(n == 0 ? F_TRUE : F_FALSE);
		}
			break;

		// 0< ( n -- f)
		case OP_ZLT:
		{
			auto n = pop();
			push(n < 0 ? F_TRUE : F_FALSE);
		}
			break;

		// 0> ( n -- f)
		case OP_ZGT:
		{
			auto n = pop();
			push(n > 0 ? F_TRUE : F_FALSE);
		}
			break;

		// 0<> (n -- f)
		case OP_ZNE:
		{
			auto n = pop();
			push(n != 0 ? F_TRUE : F_FALSE);
		}
			break;

		// and (n1 n1 -- n1)
		case OP_AND:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 & n2);
		}
			break;

		// or (n1 n2 -- n1)
		case OP_OR:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 | n2);
		}
			break;

		// not (n1 -- ~n1)
		case OP_NOT:
		{
			auto n1 = pop();
			push(!n1);
		}
			break;

		// xor (n1 n2 -- n1)
		case OP_XOR:
		{
			auto n2 = pop();
			auto n1 = pop();
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
		
		// ." message " ( -- )
		case OP_DOTQUOTE:
		{
			lex('"');
			int c = getChar();	// throw away trailing end-quote
			if (c != '"')
				ungetChar(c);

			fprintf(fout, "%s\n", lval);
		}
			break;

		// equal to (n1 n2 -- bool)
		case OP_EQ:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 == n2 ? F_TRUE : F_FALSE);
		}
		break;

		// not-equal to (n1 n2 -- bool)
		case OP_NE:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 != n2 ? F_TRUE : F_FALSE);
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
			auto n = pop();
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
			auto n = pop();
			if (0 == n)
			{
				auto addr = bytecode[ip];
				ip = addr;
			}
			else
				ip++;	// skip addr
		}
			break;

		// @ (addr -- val)
		case OP_FETCH:
		{
			auto addr = pop();
			auto val = dataseg[addr];
			push(val);
		}
			break;

		// ! (addr val -- )
		case OP_STORE:
		{
			auto addr = pop();
			auto val = pop();
			dataseg[addr] = val;
		}
			break;

		// var
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
			push(w.data_addr);
		}
			break;

		// ( -- n )
		case OP_USER_VAR:
		{
			if (w.pUserNumber)
				push(*w.pUserNumber);
			else
				push(0);	// TODO - error?
		}
			break;

		// const ( -- n)
		case OP_CONST:
		{
			auto val = pop();

			lex();
			define_word_const(lval, val);
		}
			break;

		// allot ( n -- )
		case OP_ALLOT:
		{
			auto count = pop();
			
			while (count--)
				dataseg.push_back(UNDEFINED);
		}
			break;

		case OP_DEPTH:
		{
			push(stack.size());
		}
			break;

		case OP_NOP:
		{
			// do nothing!!
		}
			break;

		case OP_HALT:
		{
			break;
		}
			break;

		case OP_SPRINT:
		{
			char *pStr = (char*)bytecode[ip++];
			fprintf(fout, "%s\n", pStr);
		}
			break;

		case OP_RSHIFT:
		{
			auto shift = pop();
			auto num = pop();
			push(num >> shift);
		}
			break;

		case OP_LSHIFT:
		{
			auto shift = pop();
			auto num = pop();
			push(num << shift);
		}
			break;

		case OP_WORDS:
		{
			for (auto iter = dict.begin(); iter != dict.end(); iter++)
			{
				fprintf(fout, "%s\n", iter->first.c_str());
			}
			fprintf(fout, "\nThere are %d WORDS in the dictionary.\n", dict.size());
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	return F_TRUE;
}
