\ core-tests.fth - test the core Words
\ Copyright 2022 Mark Seminatore. All rights reserved.

Test-group
    ." Test drop "
    T{ 1 2 drop }T 1 ==

    ." Test swap "
    T{ 1 2 swap }T 2 1 ==

    ." Test dup "
    T{ 1 dup }T 1 1 ==

    ." Test max "
    T{ 1 2 max }T 2 ==
    T{ 2 1 max }T 2 ==
    T{ -2 -1 max }T -1 ==

    ." Test min "
    T{ 1 2 min }T 1 ==
    T{ 2 1 min }T 1 ==
    T{ -2 -1 min }T -2 ==

    ." Test negate "
    T{ 1 negate }T -1 ==
    T{ -1 negate }T 1 ==
    T{ 0 negate }T 0 ==

    ." Test nip "
    T{ 1 2 nip }T 2 ==

    ." Test not "
    T{ 0 not }T 1 ==
    T{ 1 not }T 0 ==

    ." Test or "
    T{ 0 0 or }T 0 ==
    T{ 1 0 or }T 1 ==
    T{ 0 1 or }T 1 ==
    T{ 1 1 or }T 1 ==
    T{ 0 255 or }T 255 ==

    ." Test xor "
    T{ 0 0 xor }T 0 ==
    T{ 0 1 xor }T 1 ==
    T{ 1 0 xor }T 1 ==
    T{ 1 1 xor }T 0 ==

    ." Test 2dup "
    T{ 1 2 2dup }T 1 2 1 2 ==
    
    ." Test ?dup "
    T{ 1 ?dup }T 1 1 ==
    T{ 0 ?dup }T 0 ==

    ." Test */ "
    T{ 1 2 2 */ }T 1 ==

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
    
    ." Test and "
    T{ 0 0 and }T 0 ==
    T{ 1 0 and }T 0 ==
    T{ 0 1 and }T 0 ==
    T{ 1 1 and }T 1 ==
    T{ 0 255 and }T 0 ==

    ." Test over "
    T{ 1 2 over }T 1 2 1 ==

    ." Test pow "
    T{ 1 2 pow }T 1 ==
    T{ 2 2 pow }T 4 ==

    ." Test rot "
    T{ 1 2 3 rot }T 2 3 1 ==

    ." Test tuck "
    T{ 1 2 tuck }T 2 1 2 ==

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

    ." Test mod "
    T{ 0 1 mod }T 0 ==
    T{ 1 1 mod }T 0 ==
    T{ 2 1 mod }T 0 ==

    ." Test /mod "
    T{ 3 2 /mod }T 1 1 ==
    T{ 1 1 /mod }T 1 0 ==
    T{ 2 1 /mod }T 2 0 ==

    ." Test 1+ "
    T{ 0 1+ }T 1 ==
    T{ -1 1+ }T 0 ==
    T{ 1 1+ }T 2 ==

    ." Test 1- "
    T{ 0 1- }T -1 ==
    T{ -1 1- }T -2 ==
    T{ 1 1- }T 0 ==

Test-end
