# Welcome to Firth!
**Firth** is an experimental and simple programming language inspired by Forth.

> If you are new to FORTH, you can learn a lot of the basics at 
> [Easy Forth](https://skilldrick.github.io/easyforth/).

I created **Firth** primarily as an exercise in learning Forth. And also to have 
my own environment for experimentaiton. The original plan was to create my 
own Forth system, written in C++. However, along the way I decided that there 
were likely to be a few things about Forth that I probably wanted to change. 
Mainly small things to make the syntax a little bit easier for beginners, 
including myself.

> If you are familiar with Forth you may find this strange since a key feature 
> of Forth is the ability to completely redefine existing behavior.

Rather than risk creating a version of Forth that might not be compatible with 
existing Forth code, I decided to make a language heavily influenced by Forth.

## Basics of the language

In **Firth** as in Forth there are really only two key concepts. There are `Numbers` 
and there are `Words`.

### Numbers

Everything that is not a `Word` is a `Number`. Numbers largely live on the stack. 
Parameters (or arguments) to words are usually passed on the stack. The result of 
a word is placed on the stack.

### Words

`Words` are just a different name for functions. It is very easy to create new 
`Words` in **Firth**. To do so looks like this:

```Forth
func add + ;
 ok
```

This creates a new `Word` named `add` that adds the top two stack entries and
puts the result on the stack.

### Built-in Words

There are a (growing) number of basic words that have already been defined in **Firth**. 
They are:

Word | Description
---- | -----------
: | begin definition of new word
func | alias for ':'
fn | alias for ':'
def | alias for ':'
; | end definition of new word
\+ | addition
\- | subtraction
\* | multiplcation
/ | division
< | less than comparison
\> | greater than comparison
= | equivalence comparison
and | bitwise AND
or | bitwise OR
not | bitwise NOT
dup | duplicate top stack entry
swap | swap top two stack entries
drop | discard top stack entry
\. | print top stack entry
emit | print top stack entry as ASCII
CR | print a carriage return
