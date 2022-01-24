\ Core.fth - core word library for Firth
\ Copyright 2022 Mark Seminatore. All rights reserved.

( n1 n2 -- n2 )
func NIP SWAP DROP ;

( --  c)
func BL 32 EMIT ;

( -- )
func SPACE BL EMIT ;
func TAB 9 EMIT ;
func LF 10 EMIT ;
func BEL 7 EMIT ;
func CR 13 EMIT 10 EMIT ;

\ toggles for hex/decimal output
func HEX 1 _hexmode ! ;
func DECIMAL 0 _hexmode ! ;

\ define a convenience word for printing the TOS ( n -- )
func PRINT . ;

\ sqr the TOS ( n -- n )
func SQR DUP * ;

\ leave the current code pointer on the stack
\ ( -- n )
func HERE CP @ ;

( n -- n )
func CELLS 1 * ;

( n1 n2 -- n1 n2 n1 n2 )
func 2DUP OVER OVER ;

( n1 n2 -- )
func 2DROP DROP DROP ;

( -- )
func RDROP R> DROP ;

( n1 n2 -- n2 n1 n2 )
func TUCK SWAP OVER ;

( n1 -- 0 | n1 n1 )
func ?DUP DUP DUP 0= IF DROP THEN ;

( n1 n2 -- n1|n2 ) 
func MIN OVER OVER - 0< IF DROP ELSE NIP THEN ;

( n1 n2 -- n1|n2 )
func MAX OVER OVER - 0> IF DROP ELSE NIP THEN ;

( n -- -n )
func NEGATE -1 * ;

( n -- |n| )
func ABS DUP 0< IF -1 * THEN ;

( n -- n++ )
func 1+ 1 + ;

( n -- n-- )
func 1- 1 - ;

\ boolean values
0 const FALSE
-1 const TRUE

\ number ranges
2147483647 const MAX-INT
-2147483646 const MIN-INT
2147483648 const MSB

\ func IF BZ [ HERE ] ;

\ func THEN [ HERE SWAP ! ] ;
