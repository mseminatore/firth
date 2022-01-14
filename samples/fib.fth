\ fib.fth - Fibonacci sequence in Firth
\ Copyright 2022 Mark Seminatore. All rights reserved.

func fib dup
    0<> IF dup 1 
            <> IF
                0 1 rot 1- 0 DO dup rot + LOOP nip
            THEN
        THEN
;