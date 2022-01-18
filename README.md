# Welcome to Firth!
**Firth** is an experimental programming language heavily inspired by Forth.

> If you are new to Forth, you can learn a lot of the basics at 
> [Easy Forth](https://skilldrick.github.io/easyforth/). I also highly recommend
> [Starting Forth](https://www.forth.com/starting-forth) by Leo Brodie of
> Forth, Inc.

I like learning, and creating, programming languages. I created **Firth** 
primarily as an exercise in learning Forth. And also to have my own environment 
for experimentaiton. The original plan was to create my own Forth system, 
written in C++. However, along the way I decided that there were likely to be 
a few things about Forth that I probably wanted to change. Mainly small things 
to make the syntax a little bit easier for beginners, including myself.

> If you are already familiar with Forth you may find the idea of creating a 
> new Forth-like language to be strange. After all, a key feature of Forth 
> is the ability to completely redefine existing behavior. I 
> wanted to retain the ability to break basic Forth compatibility when and 
> where I want.

Rather than risk creating a version of Forth that might not be compatible with 
existing Forth code, I decided to make a language heavily influenced by and 
largely compatible with Forth.

## Basics of the Firth language

In **Firth**, as in Forth, there are really only two key concepts. There are `Numbers` 
and there are `Words`.

### Numbers

Everything that is not a `Word` is a `Number`. Numbers largely live on the stack. 
When you type a number Firth pushes it onto the stack. Parameters (or arguments) 
to words are usually passed on the stack. The result of a word is usually placed 
on the stack.

Arithmetic in Firth, as in Forth, uses Reverse Polish Notation (RPN). To add two
numbers together you would write:

```Forth
> 1 2 +
  ok

> .S
Top -> [ 3 ] 
  ok
```

This would push the number 1 and then the number 2 onto the stack. And then it 
would add those two numbers together, replacing them with the result on the 
stack. The built-in `Word` called `.S` prints out, without modifying, the 
contents of the stack.

### Words

`Words` are really just another name for functions. In **Firth** it is very easy to create new 
`Words`. Let's create a `Word` for addition. To do so looks like this:

```Forth
> func add + ;
  ok

> 1 2 add
  ok

> print
3  ok
```

This creates a new `Word` named `add` that calls `+` to add the top two stack 
entries and put the result on the stack. The built-in `Word` called `print` 
prints the top of stack.

### Firth Examples

Below are a few examples of Firth in action. First, the canonical hello world 
program.

```Forth
> func hello ." Hello World! "
  ok

> hello
Hello World!
  ok
```

Next, we define a new `Word` called `fib` that computes the Fibonacci sequence 
for the number currently on the stack.

```Forth
> func fib DUP
    0<> IF DUP 1 
            <> IF
                0 1 ROT 1- 0 DO DUP ROT + LOOP NIP
            THEN
        THEN
;
  ok

> 9 fib print
34  ok
```
### Embedding Firth

Firth is designed to be very easy to embed into other apps. Doing so requires
integration of one .cpp and one .h file and just a few Firth API calls. Adding
in the optional floating point support is one additional API call.

The file *main.cpp* demonstrates how to initialize Firth and add custom
`Words` for a constant, a variable, and a native function. The important excerpts
of *main.cpp* are shown below.

```C++

#include "firth.h"

Firth *g_pFirth = NULL;

// custom word functions
static int isEven(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) == 0 ? F_TRUE : F_FALSE);

	return F_TRUE;
}

static int isOdd(Firth *pFirth)
{
	auto n = pFirth->pop();
	pFirth->push((n % 2) ? F_TRUE : F_FALSE);

	return F_TRUE;
}

// register our collection of custom words
static const struct FirthWordSet myWords[] =
{
	{ "even?", isEven },
	{ "odd?", isOdd },
	{ nullptr, nullptr }
};

FirthNumber tickCount;

// examples of calling Firth from native code
void callFirth(Firth *pFirth)
{
    // do_word is a set of convenience methods to push 
    // 1, 2, or 3 parameters on stack and execute a word
    g_pFirth->do_word("+", 1, 2);
	
	// execute any defined word, no passed parameters
    g_pFirth->exec_word(".");
}

int main()
{
    g_pFirth = new Firth();

    // load core libraries
    g_pFirth->loadCore();

    // add custom words that can be called from Firth
    g_pFirth->register_wordset(myWords);

    // add a const and a var
    g_pFirth->define_word_const("APP.VER", 1);
    g_pFirth->define_word_var("System.Tick", &tickCount);

    // parse Firth
    while (g_pFirth->parse());

    return 0;
}

```

### Testing Firth

I have created a small test harness to allow testing words in Firth. This
framework is called *test.fth* and is located in the **test** sub-folder. You
load the framework as follows:

```Forth
> include test\test.fth
  ok

```

I have also created a set of unit tests for the core Firth `Words`. The unit 
tests are also found in the **test** sub-folder in a file called 
*core-tests.fth*. Once again you load the tests by including the file. 

> I found that having unit tests was critical to the development of this 
> project. It allowed me to ensure that newly added words functioned correctly.
> But even more importantly, having a rich set of unit tests allowed me to 
> re-factor and optimize code with confidence.

The *core-tests.fth* file file both defines and runs the unit tests. If all goes well a message will be 
displayed saying that *All tests passed!*

```Forth
> include test\core-tests.fth
Test drop
        1 √
Test swap
        2 √
Test dup
        3 √
Test max
        4 √
        5 √
        6 √
Test min
        7 √
        8 √
        9 √
Test negate
        10 √
        11 √
        12 √
Test abs
        13 √
        14 √
        15 √
Test nip
        16 √
Test not
        17 √
        18 √
Test or
        19 √
        20 √
        21 √
        22 √
        23 √
Test xor
        24 √
        25 √
        26 √
        27 √
Test 2dup
        28 √
Test 2drop
        29 √
Test ?dup
        30 √
        31 √
Test */
        32 √
        33 √
        34 √
Test <
        35 √
        36 √
        37 √
Test >
        38 √
        39 √
        40 √
Test =
        41 √
        42 √
Test <>
        43 √
        44 √
Test 0=
        45 √
        46 √
Test 0<
        47 √
        48 √
        49 √
Test 0>
        50 √
        51 √
        52 √
Test 0<>
        53 √
        54 √
Test and
        55 √
        56 √
        57 √
        58 √
        59 √
Test over
        60 √
Test pow
        61 √
        62 √
Test rot
        63 √
Test tuck
        64 √
Test +
        65 √
        66 √
        67 √
Test -
        68 √
        69 √
        70 √
Test *
        71 √
        72 √
        73 √
Test /
        74 √
        75 √
        76 √
Test mod
        77 √
        78 √
        79 √
Test /mod
        80 √
        81 √
        82 √
Test sqr
        83 √
        84 √
        85 √
        86 √
Test 1+
        87 √
        88 √
        89 √
Test 1-
        90 √
        91 √
        92 √
Test DO LOOP
        93 √
Test depth
        94 √
        95 √
        96 √
Test IF ELSE THEN
        97 √
        98 √
        99 √
        100 √
        101 √
        102 √
All tests passed!
```

### Built-in Words

There are a (growing) number of basic `Words` that have already been defined in **Firth**. 
They are:

Word | Description | Stack effects
---- | ----------- | -------------
ABS | take absolute value of TOS | ( n -- \|n\| )
AGAIN | loop back to BEGIN | ( -- )
ALLOT | reserve n extra cells for array | ( n -- )
AND | bitwise AND | ( n1 n2 -- n3 )
BEGIN | start an indefinite loop
BEL | emits a BEL char | ( -- )
BL | prints a space | ( -- )
CELLS | calculate cell count for array size | ( n -- n )
CONST | define a new constant | ( n -- )
CR | print a carriage return | ( -- )
DO | start a definite loop | ( -- )
DROP | discard TOS | ( n -- )
DUP | duplicate TOS | ( n -- n n )
ELSE | start of else clause | ( -- )
EMIT | print TOS as ASCII | ( n -- )
EXIT | exit from current loop | ( -- )
FALSE | constant representing logical false | ( -- f )
FUNC | begin definition of new word | ( -- )
I | put current loop index on the stack | ( -- n )
INCLUDE | load and parse the given Firth file | ( -- )
IF | start a conditional | ( f -- )
LF | print a line feed | ( -- )
LOOP | end of definite loop | ( -- )
LSHIFT | logical shift left n, u places | ( n1 u -- n2 )
MAX | leave greater of top two stack entries | ( n1 n2 -- n1|n2 )
MAX-INT | puts largest representable int value on stack | ( -- n )
MIN | leave lesser of top two stack entries | ( n1 n2 -- n1|n2 )
MIN-INT | puts smallest representable int value on stack | ( -- n )
MOD | compute remainder | ( n1 n2 -- n3 )
NEGATE | change sign of TOS | ( n -- -n )
NIP | discard the second entry on stack | ( n1 n2 -- n2 )
NOT | bitwise NOT | ( n1 n2 -- n3 )
OR | bitwise OR | ( n1 n2 -- n3 )
OVER | dupe the second stack entry to the top | ( n1 n2 -- n1 n2 n1 )
POW | raise x to power of y | ( x y -- x^y )
REPEAT | loop back to BEGIN | ( -- )
ROT | rotate the top 3 stack entries | ( n1 n2 n3 -- n2 n3 n1 )
RSHIFT | logical shift right n, u places | ( n1 u -- n2 )
SWAP | swap top two stack entries| ( n1 n2 -- n2 n1 )
TAB | prints a tab char | ( -- )
THEN | end of IF conditional | ( --  )
TRUE | constant representing logical true | ( -- f )
TUCK | copy the top stack item below the second stack item | ( n1 n2 -- n2 n1 n2)
UNTIL | end of indefinite loop | ( -- )
VAR | define a new variable | ( -- )
WHILE | test whether loop condition is true | ( -- )
WORDS | list all words in the dictionary | ( -- )
XOR | bitwise XOR | ( n1 n2 -- n3 )
2DUP | duplicate top two stack entries | ( n1 n2 -- n1 n2 n1 n2 )
?DUP | duplicate TOS if it is non-zero | ( n1 -- n1\| n1 n1 )
; | end definition of new word | ( -- )
\+ | addition | ( n1 n2 -- n3 )
\- | subtraction | ( n1 n2 -- n3 )
\* | multiplcation | ( n1 n2 -- n3 )
/ | division | ( n1 n2 -- n3 )
\*/ | multiply then divide | ( n1 n2 n3 -- n4 )
/MOD | remainder and quotient | ( n1 n2 -- n3 n4 )
< | less than comparison | ( n1 n2 -- f )
\> | greater than comparison | ( n1 n2 -- f )
= | equivalence comparison | ( n1 n2 -- f )
<> | not equivalence comparison | ( n1 n2 -- f )
0= | true if TOS is zero | ( n -- f )
0< | true if TOS is less than zero | ( n -- f )
0> | true if TOS is greater than zero  | ( n -- f )
0<> | true if TOS is not equal zero | ( n -- f )
\. | print TOS | ( n -- )
\.S | print the stack contents | ( -- )
\." | print the following " delimited string | ( -- )

* **Note:** These are not yet implemented