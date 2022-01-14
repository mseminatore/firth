\ test.fth - Firth testing framework
\ Copyright 2022 Mark Seminatore. All rights reserved.

var TestCount \ the current test number
var StackDepth \ saved stack depth

: CHECKMARK 251 emit ;

\ utility words
\ report the test number to a numeric output device
: T.
 . \ output last successful test #
;

\ halt the system
: halt
    BEGIN AGAIN
;

\ compute h1 by hashing x1 and h0
: hash ( x1 h0 -- h1 )
    swap 1+ xor
;

\ hash n items from the stack and return the hash code
: hash-n ( x1 x2 ... xn n -- h )
    0 >R
    BEGIN
        dup 0 >
    WHILE
        swap R> hash >R
        1-
    REPEAT
    drop R>
;

\ start a test group
: Test-group
    0 TestCount !
;

\ start an individual unit test
: T{ ( -- )
    TestCount @ 1+ dup TAB T. TestCount ! 
    depth StackDepth !
;

\ finish a unit test,
: }T ( y1 y2 ... yn -- hy )
    depth StackDepth @ - ( y1 y2 ... yn Ny )
    hash-n ( hy )
    depth StackDepth ! ( hy )
;

\ compare actual output with expected output
: == ( hy x1 x2 ... xn -- )
    depth StackDepth @ - ( hy x1 x2 .. xn Nx )
    hash-n ( hy hx )
    = 0= IF halt ELSE BL CHECKMARK THEN CR
;

\ end of test group
: Test-end ( -- )
    65535 ( 0xFFFF ) T.
;
