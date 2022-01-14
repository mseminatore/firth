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

> If you are already familiar with Forth you may find this strange since a key 
> feature of Forth is the ability to completely redefine existing behavior. I 
> wanted to retain the ability to break basic Forth compatibility.

Rather than risk creating a version of Forth that might not be compatible with 
existing Forth code, I decided to make a language heavily influenced by Forth.

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
1 2 +
```

This would push the number 1 and then the number 2 onto the stack. And then it 
would add those two numbers together, replacing them with the result on the 
stack.

### Words

`Words` are just another name for functions. It is very easy to create new 
`Words` in **Firth**. To do so looks like this:

```Forth
func add + ;
 ok

1 2 add
```

This creates a new `Word` named `add` that calls `+` to add the top two stack entries and
put the result on the stack.

### Built-in Words

There are a (growing) number of basic `Words` that have already been defined in **Firth**. 
They are:

Word | Description | Stack effects
---- | ----------- | -------------
abs | take absolute value of TOS | ( n -- \|n\| )
AGAIN | loop back to BEGIN | ( -- )
allot | reserve n extra cells for array | ( n -- )
and | bitwise AND | ( n1 n2 -- n3 )
BEGIN | start an indefinite loop
BEL | emits a BEL char | ( -- )
BL | prints a space | ( -- )
cells | calculate cell count for array size | ( n -- n )
const | define a new constant | ( n -- )
CR | print a carriage return | ( -- )
DO | start a definite loop | ( -- )
drop | discard TOS | ( n -- )
dup | duplicate TOS | ( n -- n n )
ELSE | start of else clause | ( -- )
emit | print TOS as ASCII | ( n -- )
EXIT | exit from current loop | ( -- )
FALSE | constant representing logical false | ( -- f )
func | begin definition of new word | ( -- )
I | put current loop index on the stack | ( -- n )
include | load and parse the given Firth file | ( -- )
IF | start a conditional | ( f -- )
LF | print a line feed | ( -- )
LOOP | end of definite loop | ( -- )
max | leave greater of top two stack entries | ( n1 n2 -- n1|n2 )
MAX-INT | puts largest representable int value on stack | ( -- n )
min | leave lesser of top two stack entries | ( n1 n2 -- n1|n2 )
MIN-INT | puts smallest representable int value on stack | ( -- n )
mod | compute remainder | ( n1 n2 -- n3 )
negate | change sign of TOS | ( n -- -n )
nip | discard the second entry on stack | ( n1 n2 -- n2 )
not | bitwise NOT | ( n1 n2 -- n3 )
or | bitwise OR | ( n1 n2 -- n3 )
over | dupe the second stack entry to the top | ( n1 n2 -- n1 n2 n1 )
pow | raise x to power of y | ( x y -- x^y )
REPEAT | loop back to BEGIN | ( -- )
rot | rotate the top 3 stack entries | ( n1 n2 n3 -- n2 n3 n1 )
swap | swap top two stack entries| ( n1 n2 -- n2 n1 )
TAB | prints a tab char | ( -- )
THEN | end of IF conditional | ( --  )
TRUE | constant representing logical true | ( -- f )
tuck | copy the top stack item below the second stack item | ( n1 n2 -- n2 n1 n2)
UNTIL | end of indefinite loop | ( -- )
var | define a new variable | ( -- )
WHILE | test whether loop condition is true | ( -- )
xor | bitwise XOR | ( n1 n2 -- n3 )
2dup | duplicate top two stack entries | ( n1 n2 -- n1 n2 n1 n2 )
?dup | duplicate TOS if it is non-zero | ( n1 -- n1\| n1 n1 )
; | end definition of new word | ( -- )
\+ | addition | ( n1 n2 -- n3 )
\- | subtraction | ( n1 n2 -- n3 )
\* | multiplcation | ( n1 n2 -- n3 )
/ | division | ( n1 n2 -- n3 )
\*/ | multiply then divide | ( n1 n2 n3 -- n4 )
/mod | remainder and quotient | ( n1 n2 -- n3 n4 )
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