; Test #3 Cancellation of Conditional IFAL Through Call

org 0x1000
  alpha byte  0x32          ;1000
  beta  byte  0x34          ;1001

org 0x2000
  main
    cmp.b &alpha, &beta     ;2000, 2002, 2004
    ifal  word    0x1FED    ;2006, IFAL inst with 3 TC and 1 FC
;If equal
    mov   #2, &alpha        ;2008, 200A
    call  #subroutine
    sxt   &beta
;Else
    add.b #1, &alpha

org 0x2100
  subroutine
    xor &alpha, &beta
    mov @SP+, PC

end 0x2000
