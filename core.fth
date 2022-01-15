\ Core.fth - core word library for Firth
\ Copyright 2022 Mark Seminatore. All rights reserved.

( n1 n2 -- n2 )
func nip swap drop ;

( --  c)
func BL 32 emit ;

( -- )
func SPACE BL emit ;
func TAB 9 emit ;
func LF 10 emit ;
func BEL 7 emit ;
func CR 13 emit 10 emit ;

\ define a convenience word for printing the TOS ( n -- )
func print . ;

\ sqr the TOS ( n -- n )
func sqr dup * ;

func HERE CP @ ;

( n -- n )
func cells 1 * ;

( n1 n2 -- n1 n2 n1 n2 )
func 2dup over over ;

( n1 n2 -- )
func 2drop drop drop ;

( -- )
func rdrop R> drop ;

(n1 n2 -- n2 n1 n2 )
func tuck swap over ;

( n1 -- 0 | n1 n1 )
func ?dup dup dup 0= IF drop THEN ;

( n1 n2 -- n1|n2 ) 
func min over over - 0< IF drop ELSE nip THEN ;

( n1 n2 -- n1|n2 )
func max over over - 0> IF drop ELSE nip THEN ;

( n -- -n )
func negate -1 * ;

( n -- |n| )
func abs dup 0< IF -1 * THEN ;

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

\ func THEN [ HERE swap ! ] ;
