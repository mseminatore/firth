\ core-tests.fth - test the core Firth Words
\ Copyright 2022 Mark SeMINatore. All rights reserved.

\
\ Some test cases borrowed from https://forth-stANDard.org/stANDard/core/
\
include test.fth

Test-group
    ." Test DROP "
    T{ 1 2 DROP }T 1 ==

    ." Test SWAP "
    T{ 1 2 SWAP }T 2 1 ==

    ." Test DUP "
    T{ 1 DUP }T 1 1 ==

    ." Test MAX "
    T{ 1 2 MAX }T 2 ==
    T{ 2 1 MAX }T 2 ==
    T{ -2 -1 MAX }T -1 ==

    ." Test MIN "
    T{ 1 2 MIN }T 1 ==
    T{ 2 1 MIN }T 1 ==
    T{ -2 -1 MIN }T -2 ==

    ." Test NEGATE "
    T{ 1 NEGATE }T -1 ==
    T{ -1 NEGATE }T 1 ==
    T{ 0 NEGATE }T 0 ==

    ." Test ABS "
    T{ -1 ABS }T 1 ==
    T{ 1 ABS }T 1 ==
    T{ 0 ABS }T 0 ==

    ." Test NIP "
    T{ 1 2 NIP }T 2 ==

    ." Test NOT "
    T{ 0 NOT }T 1 ==
    T{ 1 NOT }T 0 ==

    ." Test OR "
    T{ 0 0 OR }T 0 ==
    T{ 1 0 OR }T 1 ==
    T{ 0 1 OR }T 1 ==
    T{ 1 1 OR }T 1 ==
    T{ 0 255 OR }T 255 ==

    ." Test XOR "
    T{ 0 0 XOR }T 0 ==
    T{ 0 1 XOR }T 1 ==
    T{ 1 0 XOR }T 1 ==
    T{ 1 1 XOR }T 0 ==

    ." Test 2DUP "
    T{ 1 2 2DUP }T 1 2 1 2 ==
    
    ." Test 2DROP "
    T{ 1 2 3 2DROP }T 1 ==

    ." Test ?DUP "
    T{ 1 ?DUP }T 1 1 ==
    T{ 0 ?DUP }T 0 ==

    ." Test */ "
    T{ 1 2 2 */ }T 1 ==
    T{ MAX-INT 2 MAX-INT */ }T 2 ==
    T{ MIN-INT 2 MIN-INT */ }T 2 ==

    ." Test < "
    T{ 1 2 < }T -1 ==
    T{ 2 1 < }T 0 ==
    T{ -1 1 < }T -1 ==

    ." Test > "
    T{ 1 2 > }T 0 ==
    T{ 2 1 > }T -1 ==
    T{ -1 1 > }T 0 ==

    ." Test = "
    T{ 1 1 = }T -1 ==
    T{ 1 0 = }T 0 ==

    ." Test <> "
    T{ 1 1 <> }T 0 ==
    T{ 1 0 <> }T -1 ==

    ." Test 0= "
    T{ 1 0= }T 0 ==
    T{ 0 0= }T -1 ==

    ." Test 0< "
    T{ 1 0< }T 0 ==
    T{ -1 0< }T -1 ==
    T{ 0 0< }T 0 ==

    ." Test 0> "
    T{ 1 0> }T -1 ==
    T{ -1 0> }T 0 ==
    T{ 0 0> }T 0 ==

    ." Test 0<> "
    T{ 1 0<> }T -1 ==
    T{ 0 0<> }T 0 ==
    
    ." Test AND "
    T{ 0 0 AND }T 0 ==
    T{ 1 0 AND }T 0 ==
    T{ 0 1 AND }T 0 ==
    T{ 1 1 AND }T 1 ==
    T{ 0 255 AND }T 0 ==

    ." Test OVER "
    T{ 1 2 over }T 1 2 1 ==

    ." Test POW "
    T{ 1 2 POW }T 1 ==
    T{ 2 2 POW }T 4 ==

    ." Test ROT "
    T{ 1 2 3 ROT }T 2 3 1 ==

    ." Test TUCK "
    T{ 1 2 TUCK }T 2 1 2 ==

    ." Test + "
    T{ 1 1 + }T 2 ==
    T{ 0 1 + }T 1 ==
    T{ -1 2 + }T 1 ==
    
    ." Test - "
    T{ 1 1 - }T 0 ==
    T{ 0 1 - }T -1 ==
    T{ -1 2 - }T -3 ==
    
    ." Test * "
    T{ 1 1 * }T 1 ==
    T{ 0 1 * }T 0 ==
    T{ -1 2 * }T -2 ==
    
    ." Test / "
    T{ 1 1 / }T 1 ==
    T{ 0 1 / }T 0 ==
    T{ 3 1 / }T 3 ==

    ." Test MOD "
    T{ 0 1 MOD }T 0 ==
    T{ 1 1 MOD }T 0 ==
    T{ 2 1 MOD }T 0 ==

    ." Test /MOD "
    T{ 3 2 /MOD }T 1 1 ==
    T{ 1 1 /MOD }T 1 0 ==
    T{ 2 1 /MOD }T 2 0 ==

    ." Test SQR "
    T{ 0 SQR }T 0 ==
    T{ 1 SQR }T 1 ==
    T{ 2 SQR }T 4 ==
    T{ -2 SQR }T 4 ==

    ." Test 1+ "
    T{ 0 1+ }T 1 ==
    T{ -1 1+ }T 0 ==
    T{ 1 1+ }T 2 ==

    ." Test 1- "
    T{ 0 1- }T -1 ==
    T{ -1 1- }T -2 ==
    T{ 1 1- }T 0 ==

    ." Test LSHIFT "
    T{ 1 0 LSHIFT }T 1 ==
    T{ 1 1 LSHIFT }T 2 ==
    T{ 1 2 LSHIFT }T 4 ==
    \ T{ MSB 1 LSHIFT }T 0 ==

    ." Test RSHIFT "
    T{ 1 0 RSHIFT }T 1 ==
    T{ 1 1 RSHIFT }T 0 ==
    T{ 2 1 RSHIFT }T 1 ==
    T{ 4 2 RSHIFT }T 1 ==
    \ T{ MSB 1 RSHIFT 2 * }T MSB ==

    ." Test DO LOOP "
    T{ func DL1 3 0 DO I LOOP ; DL1 }T 0 1 2 ==
    T{ func DL2 10 7 FOR I LOOP ; DL2 }T 7 8 9 ==

    ." Test FOR LOOP "
    T{ func DL3 3 0 FOR I LOOP ; DL3 }T 0 1 2 ==

    ." Test FOR +LOOP "
    T{ func DL4 6 0 FOR I 2 +LOOP ; DL4 }T 0 2 4 ==

    ." Test DEPTH "
    T{ 0 1 DEPTH }T 0 1 2 ==
    T{ 0 DEPTH }T 0 1 ==
    T{ DEPTH }T 0 ==

    ." Test IF ELSE THEN "
    func GI1 IF 123 THEN ;
    func GI2 IF 123 ELSE 234 THEN ;
    T{ 0 0 GI1 }T 0 ==
    T{ 1 GI1 }T 123 ==
    T{ -1 GI1 }T 123 ==
    T{ 0 GI2 }T 234 ==
    T{ 1 GI2 }T 123 ==
    T{ -1 GI2 }T 123 ==

    ." Test IF ELSE ENDIF "
    func GI3 IF 123 ENDIF ;
    func GI4 IF 123 ELSE 234 ENDIF ;
    T{ 0 0 GI3 }T 0 ==
    T{ 1 GI3 }T 123 ==
    T{ -1 GI3 }T 123 ==
    T{ 0 GI4 }T 234 ==
    T{ 1 GI4 }T 123 ==
    T{ -1 GI4 }T 123 ==

 \   ." Test >R R> "
 \   T{ 0 >R R> }T 0 ==
 \   T{ 1 2 3 >R R> }T 1 2 3 ==

 \   ." Test 2>R 2R> "
 \   T{ 0 1 2>R 2R> }T 0 1 ==
 \   T{ 1 2 3 2>R 2R> }T 1 2 3 ==

Test-end
