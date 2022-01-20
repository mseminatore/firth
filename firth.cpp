#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>

#include "firth.h"

//
static void firth_default_output(char *s)
{
	puts(s);
}

//
// Create the default environment
//
Firth::Firth(unsigned data_limit)
{
	firth_print = firth_default_output;

	halted = false;

	fin = stdin;
	txtInput = nullptr;

	this->data_limit = data_limit;
	DP = pDataSegment = new FirthNumber[data_limit];

	CP = DP++;
	*CP = 0;

	ip = 0;

	hexmode = 0;

	interpreter = true;

	// special opcode that tells us when execution is finished
	emit(OP_DONE);

	// math words
	define_word("+", OP_PLUS);
	define_word("-", OP_MINUS);
	define_word("*", OP_MUL);
	define_word("/", OP_DIV);
	define_word("MOD", OP_MOD);
	define_word("/MOD", OP_DIVMOD);
	define_word("*/", OP_MULDIV);
	define_word("POW", OP_POW);

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
	define_word("AND", OP_AND);
	define_word("OR", OP_OR);
	define_word("NOT", OP_NOT);
	define_word("XOR", OP_XOR);
	define_word("LSHIFT", OP_LSHIFT);
	define_word("RSHIFT", OP_RSHIFT);

	// stack words
	define_word("DUP", OP_DUP);
	define_word("SWAP", OP_SWAP);
	define_word("DROP", OP_DROP);
	define_word("ROT", OP_ROT);
	define_word("OVER", OP_OVER);
	define_word(">R", OP_TO_R);
	define_word("R>", OP_FROM_R);
	define_word("R@", OP_RFETCH);

	// IO words
	define_word(".", OP_PRINT);
	define_word("EMIT", OP_EMIT);
	define_word(".S", OP_DOTS);
	define_word(".\"", OP_DOTQUOTE);

	// variable and constant words
	define_word("VAR", OP_VAR);
	define_word("VARIABLE", OP_VAR);			// for Forth compat
	define_word("__var_impl", OP_VAR_IMPL);
	define_word("CONST", OP_CONST);
	define_word("CONSTANT", OP_CONST);			// for Forth compat
	define_word("@", OP_FETCH);
	define_word("!", OP_STORE);
	define_word("ALLOT", OP_ALLOT);

#if FTH_INCLUDE_FLOAT == 1
	// float support
	define_word("FVAR", OP_FVAR);
	define_word("FVARIABLE", OP_FVAR);
	define_word("FCONST", OP_FCONST);
#endif

	// compiler words
	define_word(":", OP_FUNC);
	define_word("FUNC", OP_FUNC);
	define_word("FN", OP_FUNC);
	define_word("DEF", OP_FUNC);
	define_word("INCLUDE", OP_LOAD);
	define_word("DEPTH", OP_DEPTH);
	define_word("WORDS", OP_WORDS);

	// pre-defined variables
	define_word_var("CP", CP);

	// hide internal words
	make_hidden("__var_impl", true);
}

// Firth formatted output function
void Firth::firth_printf(char *format, ...)
{
	char buf[FTH_MAX_PRINTF_SIZE];
	va_list valist;

	va_start(valist, format);
		vsprintf(buf, format, valist);
	va_end(valist);

	firth_print(buf);
}

//
int Firth::load(const std::string &file)
{
	FILE *f = fopen(file.c_str(), "rt");
	if (!f)
		return FTH_FALSE;

	set_input_file(f);
	
	while(parse());

	fclose(f);
	
	return FTH_TRUE;
}

// write given opcode to code segment
void Firth::emit(FirthInstruction op)
{
	bytecode.push_back(op);

	// update CP
	*CP = bytecode.size();
}

//
int Firth::getChar()
{
	if (txtInput)
		return *txtInput++;

	return fgetc(fin);
}

//
void Firth::ungetChar(int c)
{
	if (txtInput)
	{
		// string already has previous character so just update pointer
		txtInput--;
		return;
	}

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
	return (isdigit(c) || c == '-');
}

bool Firth::isInteger(const char *s)
{
	if (*s != '-' && *s != '+' && !isdigit(*s))
		return false;

	s++;

	for (; *s; s++)
	{
		if (!isdigit(*s))
			return false;
	}

	return true;
}

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

	if (chr == EOF || chr == '\n' || chr == 0)
		return chr;

	// look for a word
	do
	{
		*pBuf++ = chr;
	} while (chr && (chr = getChar()) != EOF && chr != delim && chr != '\n');

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
		firth_print(" Stack underflow\n");
		return 0;
	}

	auto num = stack.top();
	stack.pop();
	return num;
}

#if FTH_INCLUDE_FLOAT == 1
//
FirthFloat Firth::popf()
{
	// check for stack underflow
	if (fstack.size() == 0)
	{
		firth_print(" Float Stack underflow\n");
		return 0;
	}

	auto num = fstack.top();
	fstack.pop();
	return num;
}
#endif

//
int Firth::interpret(const std::string &token)
{
	if (token == "]")
	{
		interpreter = false;
		return FTH_TRUE;
	}

	// if the word is in the dictionary do it
	if (!exec_word(token))
	{
		// if the word is not in dictionary assume it is a number and push it on the stack
		if (isInteger(token.c_str()))
		{
			FirthNumber num = atoi(token.c_str());
			push(num);
		}
#if FTH_INCLUDE_FLOAT == 1
		else if (isdigit(token[0]) && (strchr(token.c_str(), '.') || strchr(token.c_str(), 'e')))
		{
			FirthFloat num = (FirthFloat)atof(token.c_str());
			pushf(num);
		}
#endif
		else
		{
			firth_printf("%s ?\n", token.c_str());
//			return FTH_FALSE;
		}
	}

	return FTH_TRUE;
}

//
int Firth::parse_token(const std::string &token)
{
	if (interpreter)
		return interpret(token);

	return compile(token);
}

// Parse and execute a string of Firth
int Firth::parse_string(const std::string &line)
{
	// set string pointer
	assert(txtInput == nullptr);
	txtInput = line.c_str();

	// parse a  line
	auto result = parse();
	
	txtInput = nullptr;

	return FTH_TRUE;
}

// Parse and execute a line of text
int Firth::parse()
{
	int token;
	int success = FTH_TRUE;

	if (fin == stdin)
		firth_print("\nfirth> ");

	while ((token = lex()) != '\n' && token != EOF && token != 0 && halted == false)
	{
		success = parse_token(lval);
		if (!success)
		{
			assert(false);
			return FTH_FALSE;
		}
	}

	if (token == '\n' && success == FTH_TRUE && fin == stdin)
		firth_print(" ok\n");

	if (token == EOF || token == 0 || halted)
	{
		// if reading from a file and file is finished, read from stdin
		if (fin != stdin)
		{
			fclose(fin);
			set_input_file(stdin);
		}

		return FTH_FALSE;
	}

	return FTH_TRUE;
}

//
int Firth::register_wordset(const FirthWordSet words[])
{
	for (int i = 0; words[i].wordName && words[i].func; i++)
	{
		if (FTH_FALSE == define_user_word(words[i].wordName, words[i].func))
			return FTH_FALSE;
	}

	return FTH_TRUE;
}

//
int Firth::create_word(const std::string &word, const Word &w)
{
	// TODO - we might want to succeed in overwriting existing words here
	auto result = dict.insert(std::pair<const std::string, Word>(word, w));
	if (result.second == false)
		return FTH_FALSE;

	return FTH_TRUE;
}

// Create a new builtin word
int Firth::define_word(const std::string &word, int opcode, bool compileOnly)
{
	Word w;
	w.code_addr = bytecode.size();
	w.compileOnly = compileOnly;
	w.opcode = opcode;

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	// run-time behavior
	emit(opcode);
	emit(OP_RET);

	// add opcode/word to diasm
	disasm.insert(std::pair<int, std::string>(opcode, word));

	return FTH_TRUE;
}

// Create a new native word
int Firth::define_user_word(const std::string &word, FirthFunc func, bool compileOnly)
{
	Word w;

	if (!func)
		return FTH_FALSE;

	w.nativeWord = func;
	w.compileOnly = compileOnly;
	w.opcode = OP_NATIVE_FUNC;
	w.type = OP_NATIVE_FUNC;

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	return FTH_TRUE;
}

// prevent the word from enumerating to users
int Firth::make_hidden(const std::string &word, bool flag)
{
	Word *word_obj = nullptr;

	if (!lookup_word(word, &word_obj))
		return FTH_FALSE;

	word_obj->hidden = flag;

	return FTH_TRUE;
}

// define a variable at the given address
int Firth::define_word_var(const std::string &word, FirthNumber *daddr)
{
	Word *var_word = nullptr;

	if (!lookup_word("__var_impl", &var_word))
		return FTH_FALSE;

	Word w;

	// all vars call the same run-time code
	w.code_addr = var_word->code_addr;
	w.data_addr = daddr;
	w.type = OP_VAR;
	w.opcode = OP_VAR;

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	return FTH_TRUE;
}

// initialize a new variable with given value
int Firth::define_word_var(const std::string &word, FirthNumber val)
{
	if (FTH_FALSE == define_word_var(word, DP))
		return FTH_FALSE;

	push_data(val);
	return FTH_TRUE;
}

#if FTH_INCLUDE_FLOAT == 1
//
int Firth::define_word_fvar(const std::string &word, FirthFloat *daddr)
{
	Word *var_word = nullptr;

	if (!lookup_word("__var_impl", &var_word))
		return FTH_FALSE;

	Word w;

	// all vars call the same run-time code
	w.code_addr = var_word->code_addr;
	w.data_addr = (FirthNumber*)daddr;
	w.type = OP_FVAR;
	w.opcode = OP_FVAR;

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	return FTH_TRUE;
}

//
int Firth::define_word_fvar(const std::string &word, FirthFloat val)
{
	if (FTH_FALSE == define_word_fvar(word, (FirthFloat*)DP))
		return FTH_FALSE;

	push_data((FirthNumber)val);
	return FTH_TRUE;
}

//
int Firth::define_word_fconst(const std::string &word, FirthFloat val)
{
	Word w;

	w.code_addr = bytecode.size();
	w.type = OP_FCONST;

	emit(OP_FLIT);
	emit(*(int*)&val);
	emit(OP_RET);

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	return FTH_TRUE;
}
#endif

//
int Firth::define_word_const(const std::string &word, FirthNumber val)
{
	Word w;

	w.code_addr = bytecode.size();
	w.type = OP_CONST;

	emit(OP_LIT);
	emit(val);
	emit(OP_RET);

	if (FTH_FALSE == create_word(word, w))
		return FTH_FALSE;

	return FTH_TRUE;
}

// Look word up in the dictionary
int Firth::lookup_word(const std::string &word, Word **ppWord)
{
	auto result = dict.find(word);

	if (result == dict.end())
		return FTH_FALSE;

	*ppWord = &result->second;
	return FTH_TRUE;
}

//
int Firth::compile(const std::string &token)
{
	if (token == "[")
	{
		interpreter = true;
		return FTH_TRUE;
	}

	// look for end of current word
	if (!strcmp(token.c_str(), ";"))
	{
		emit(OP_RET);
		interpreter = true;
		return FTH_TRUE;
	}

	// compile the call to the existing WORD
	Word *w = nullptr;
	if (lookup_word(token, &w))
	{
		return compile_time(*w);
	}
	else
	{
		// or compile the NUMBER
		if (isInteger(token.c_str()))
		{
			FirthNumber num = atoi(token.c_str());
			emit(OP_LIT);
			emit(num);
		}
#if FTH_INCLUDE_FLOAT == 1
		else if (isdigit(token[0]) && (strchr(token.c_str(), '.') || strchr(token.c_str(), 'e')))
		{
			FirthFloat num = (FirthFloat)atof(token.c_str());
			emit(OP_FLIT);
			emit(*(int*)&num);
		}
#endif
		else
		{
			firth_printf("%s ?\n", token.c_str());
			return FTH_FALSE;
		}
	}

	return FTH_TRUE;
}

//
int Firth::compile_time(const Word &w)
{
	switch (w.opcode)
	{
	case OP_VAR:
	{
		emit(OP_LIT);
		emit((FirthInstruction)w.data_addr);
	}
		break;

#if FTH_INCLUDE_FLOAT == 1
	case OP_FVAR:
	{
		emit(OP_FLIT);
		emit((FirthInstruction)w.data_addr);
	}
		break;
#endif

	case OP_IF:
	{
		// compile-time behavior
		emit(OP_BZ);			// emit conditional branch on zero
		push(bytecode.size());	// push current code pointer onto the stack
		emit(FTH_UNDEFINED);		// reserve space for branch address
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
		emit(OP_GOTO);						// unconditional branch
		push(bytecode.size());				// push current code pointer onto the stack for THEN
		emit(FTH_UNDEFINED);				// default to next instruction

		// patch IF branch to here, beyond the ELSE
		bytecode[dest] = bytecode.size();	// fixup branch target
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
		emit(FTH_UNDEFINED);	// placeholder for forward address
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
		emit((FirthInstruction)_strdup(lval));
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

	case OP_NATIVE_FUNC:
	{
		emit(w.opcode);
		emit((FirthInstruction)w.nativeWord);
	}
		break;

	case OP_DONE:
	default:
		emit(w.opcode);
	}

	return FTH_TRUE;
}

//
// If word is in the dictionary execute its code, ELSE error
//
int Firth::exec_word(const std::string &word)
{
	auto result = dict.find(word);

	// if WORD not found, error
	if (result == dict.end())
		return FTH_FALSE;

	Word w = result->second;

	// we can't execute compile-time only words
	if (w.compileOnly)
	{
		firth_print(" action is not a function\n");
		return FTH_FALSE;
	}

	// execute native words
	if (w.nativeWord)
	{
		return w.nativeWord(this);
	}

	// jump to the execution context (xt) for the word
	return_stack.push(ip);	// TODO is this needed?
	ip = w.code_addr;
	
	while(1)
	{
		switch (bytecode[ip++])
		{
		// execution is complete
		case OP_DONE:
		{
			ip = 0;
			return FTH_TRUE;
		}
			break;

		case OP_NATIVE_FUNC:
		{
			FirthFunc nativeWord = (FirthFunc)bytecode[ip++];
			nativeWord(this);
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
			firth_printf("%d", a);
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

			return FTH_TRUE;
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

#if FTH_INCLUDE_FLOAT == 1
		case OP_FLIT:
		{
			auto num = (FirthNumber)bytecode[ip++];
			pushf(*((FirthFloat*)&num));
		}
			break;
#endif

		// ( c -- )
		case OP_EMIT:
		{
			auto c = pop();
			firth_printf("%c", c);
		}
			break;

		// less than (n1 n2 -- bool)
		case OP_LT:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 < n2 ? FTH_TRUE : FTH_FALSE);
		}
			break;

		// greater than (n1 n2 -- bool)
		case OP_GT:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 > n2 ? FTH_TRUE : FTH_FALSE);
		}
			break;

		// 0= ( n -- f)
		case OP_ZEQ:
		{
			auto n = pop();
			push(n == 0 ? FTH_TRUE : FTH_FALSE);
		}
			break;

		// 0< ( n -- f)
		case OP_ZLT:
		{
			auto n = pop();
			push(n < 0 ? FTH_TRUE : FTH_FALSE);
		}
			break;

		// 0> ( n -- f)
		case OP_ZGT:
		{
			auto n = pop();
			push(n > 0 ? FTH_TRUE : FTH_FALSE);
		}
			break;

		// 0<> (n -- f)
		case OP_ZNE:
		{
			auto n = pop();
			push(n != 0 ? FTH_TRUE : FTH_FALSE);
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

			firth_printf("Top -> [ ");
			while(s.size())
			{
				firth_printf("%d ", s.top());
				s.pop();
			}
			firth_print("]\n");
		}
			break;
		
		// ." message " ( -- )
		case OP_DOTQUOTE:
		{
			lex('"');
			int c = getChar();	// throw away trailing end-quote
			if (c != '"')
				ungetChar(c);

			firth_printf("%s\n", lval);
		}
			break;

		// equal to (n1 n2 -- bool)
		case OP_EQ:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 == n2 ? FTH_TRUE : FTH_FALSE);
		}
		break;

		// not-equal to (n1 n2 -- bool)
		case OP_NE:
		{
			auto n2 = pop();
			auto n1 = pop();
			push(n1 != n2 ? FTH_TRUE : FTH_FALSE);
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
			{
				set_input_file(f);
				while (parse());
				fclose(f);
			}
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
			FirthNumber *pNum = (FirthNumber*)pop();
			auto val = *pNum;
			push(val);
		}
			break;

		// ! (addr val -- )
		case OP_STORE:
		{
			FirthNumber *pNum = (FirthNumber*)pop();
			auto val = pop();
			*pNum = val;
		}
			break;

		// var
		case OP_VAR:
		{
			lex();
			define_word_var(lval, FTH_UNDEFINED);
		}
			break;

#if FTH_INCLUDE_FLOAT == 1
		// fvar
		case OP_FVAR:
		{
			lex();
			define_word_fvar(lval, 0.0f);
		}
			break;
#endif

		// ( -- addr)
		case OP_VAR_IMPL:
		{
			// get addr of the variable and push it onto the stack
			push((FirthNumber)w.data_addr);
		}
			break;

		// const ( n -- )
		case OP_CONST:
		{
			auto val = pop();

			lex();
			define_word_const(lval, val);
		}
			break;

#if FTH_INCLUDE_FLOAT == 1
			// fconst ( f: n -- )
		case OP_FCONST:
		{
			auto val = popf();

			lex();
			define_word_fconst(lval, val);
		}
			break;
#endif

		// allot ( n -- )
		case OP_ALLOT:
		{
			auto count = pop();
			
			while (count--)
				push_data(FTH_UNDEFINED);
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
			halted = true;
		}
			break;

		case OP_SPRINT:
		{
			char *pStr = (char*)bytecode[ip++];
			firth_printf("%s\n", pStr);
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
			int count = 0;
			for (auto iter = dict.begin(); iter != dict.end(); iter++)
			{
				Word w = iter->second;

				if (!w.hidden)
				{
					firth_printf("%s", iter->first.c_str());
					if (w.compileOnly)
						firth_print(" [compile-time]");
					if (w.nativeWord)
						firth_print(" [native word]");
					if (w.type == OP_VAR)
						firth_print(" [var]");
					if (w.type == OP_CONST)
						firth_print(" [const]");

					firth_print("\n");
					count++;
				}
			}
			firth_printf("\nThere are %d WORDS in the dictionary.\n", count);
		}
			break;

		default:
			assert(false);
			break;
		}
	}

	return FTH_TRUE;
}
