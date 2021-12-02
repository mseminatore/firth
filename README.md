# Welcome to f!
**f** is an experimental and simple programming language inspired by Forth.

> If you are new to FORTH you can learn the basics at 
> [Easy Forth](https://skilldrick.github.io/easyforth/).

I created **f** primarily as an exercise in learning Forth. And also to have 
my own environment for experimentaiton. The original plan was to create my 
own Forth system, written in C++. However, along the way I decided that there 
were likely to be a few things about Forth that I probably wanted to change. 
Mainly things to make the syntax a little bit easier for beginners.

> If you are familiar with Forth you may find this strange since a key feature 
> of Forth is the ability to completely redefine existing behavior.

Rather than create an incompatible version of Forth I decided to make a language
heavily influenced by Forth.

## Basics of the language

In **f** as in Forth there are really only two key concepts. There are `Numbers` 
and there are `Words`.

## Numbers

Everything that is not a `Word` is a `Number`. Numbers largely live on the stack. 
Parameters (or arguments) to words are usually passed on the stack. The result of 
a word is placed on the stack.

## Words

`Words` are just a different name for functions. It is very easy to create new 
`Words` in **f**. To do so looks like this:

```Forth
func add + ;
 ok
```

This creates a new `Word` named `add` that adds the top two stack entries and
puts the result on the stack.

There are a (growing) number of basic words that have been defined so far for **f**. 
They are:

Word | Description
---- | -----------
: | begin definition of new word
func | alias for ':'
; | end definition of new word
+ | addition
- | subtraction
* | multiplcation
/ | division
< | less than comparison
> | greater than comparison
= | equivalence comparison
and | bitwise AND
or | bitwise OR
not | bitwise NOT
dup | duplicate top stack entry
swap | swap top two stack entries
drop | discard top stack entry
. | print top stack entry
emit | print top stack entry as ASCII
CR | print a carriage return
