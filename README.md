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

Word | Description
---- | -----------
abs | take absolute value of TOS
AGAIN | loop back to BEGIN
allot | reserve n extra cells for array
and | bitwise AND
BEGIN | start an indefinite loop
BEL | emits a BEL char
BL | prints a space
cells | calculate cell count for array size
const | define a new constant
CR | print a carriage return
DO | start a definite loop
drop | discard top stack entry
dup | duplicate top stack entry
emit | print top stack entry as ASCII
EXIT | exit from current loop
func | begin definition of new word
| | put current loop index on the stack
LF | print a line feed
LOOP | end of definite loop
max | leave greater of top two stack entries
min | leave lesser of top two stack entries
mod | compute remainder
negate | change sign of TOS
nip | discard the second entry on stack
not | bitwise NOT
or | bitwise OR
over | dupe the second stack entry to the top
pow | raise x to power of y
REPEAT | loop back to BEGIN
rot | rotate the top 3 stack entries
swap | swap top two stack entries
TAB | prints a tab char
tuck | copy the top stack item below the second stack item
UNTIL | end of indefinite loop
var | define a new variable
WHILE | test whether loop condition is true
xor | bitwise XOR
2dup | duplicate top two stack entries
?dup | duplicate TOS if it is non-zero
; | end definition of new word
\+ | addition
\- | subtraction
\* | multiplcation
/ | division
\*/ | multiply then divide
/mod | remainder and quotient
< | less than comparison
\> | greater than comparison
= | equivalence comparison
<> | not equivalence comparison
0= | true if TOS is zero
0< | true if TOS is less than zero
0> | true if TOS is greater than zero 
0<> | true if TOS is not equal zero
\. | print top stack entry
\.S | print the stack contents

* **Note:** These are not yet implemented