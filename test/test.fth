\ test.fth - Firth testing framework
\ Copyright 2022 Mark Seminatore. All rights reserved.

\ -----------------------------------------------------------------------
\ This framework is heavily influenced and inspired by the simple-tester 
\ framework by Ulrich Hoffmann and Andrew Read, EuroForth 2019 as well as
\ the ANS Forth test harness.
\
\ See https://http://www.euroforth.org/ef19/papers/hoffmanna.pdf
\ See https://github.com/Anding/simple-tester
\ -----------------------------------------------------------------------

var TestCount \ the current test number
var StackDepth \ saved stack DEPTH

: CHECKMARK 251 emit ;

\ report the test number to a numeric output device
: T.
 . \ output last successful test #
;

\ halt the system
: Test-halt
    BEGIN AGAIN
;

\ compute h1 by hashing x1 and h0
: hash ( x1 h0 -- h1 )
    SWAP 1+ XOR
;

\ hash n items from the stack and return the hash code
: hash-n ( x1 x2 ... xn n -- h )
    0 >R
    BEGIN
        DUP 0 >
    WHILE
        SWAP R> hash >R
        1-
    REPEAT
    DROP R>
;

\ start a test group
: Test-group
    0 TestCount !
;

\ start an individual unit test
: T{ ( -- )
    TestCount @ 1+ DUP TAB T. TestCount ! 
    DEPTH StackDepth !
;

\ finish a unit test,
: }T ( y1 y2 ... yn -- hy )
    DEPTH StackDepth @ - ( y1 y2 ... yn Ny )
    hash-n ( hy )
    DEPTH StackDepth ! ( hy )
;

\ compare actual output with expected output
: == ( hy x1 x2 ... xn -- )
    DEPTH StackDepth @ - ( hy x1 x2 .. xn Nx )
    hash-n ( hy hx )
    = 0= IF Test-halt ELSE BL CHECKMARK ENDIF CR
;

\ end of test group
: Test-end ( -- )
    ." All tests passed! " \ 65535 ( 0xFFFF ) T.
;
