\ factorial.fth - compute the factorial of a value ( n -- n! )
\ Copyright 2022 Mark Seminatore. All rights reserved.

func factorial DUP 
    0<> IF DUP 1
            <> IF
                DUP 1- 
                BEGIN 
                    DUP 1- ROT ROT * SWAP DUP 0 = 
                UNTIL 
                DROP
            ENDIF
        ENDIF
;