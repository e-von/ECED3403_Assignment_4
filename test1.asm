; Rule #1 Test, True Condition, Existing Then Instructions

org 0x1000
  alpha word  0x1234
  beta  word  0x1234

org 0x2000
  main
    cmp   &alpha, &beta     ;2000, 2002, 2004
    ifeq  word    0x1F0F    ;2006, IFEQ inst with 3 TC and 3 FC
;If equal
    mov   #0, &alpha        ;2008, 200A
    mov   #1, &beta         ;200C, 200E
    sxt   beta
;Else
    mov   &beta, &alpha     ;2010, 2012, 2014
    add   #1, &alpha        ;2016, 2018
    mov   #0, &beta         ;201A, 201C
;Unconditional
    mov   #0, R3            ;Unconditional NOP for demo purposes

end 0x2000
