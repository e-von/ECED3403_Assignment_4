org 0x1000
  alpha word  0x1234
  beta  word  0x7FFF

org 0x2000
  main
    add   &alpha, &beta     ;2000, 2002, 2004
    ifne  word    0x1F1F    ;2006
;If equal
    mov   #0, &alpha        ;2008, 200A
    jmp   sbr               ;200C
    sxt   beta              ;200E
;Else
    mov   &beta, &alpha     ;2010, 2012, 2014
    add   #1, &alpha        ;2016, 2018
    mov   #0, &beta         ;201A, 201C

org 0x2100
  sbr
    xor alpha, beta
    jmp $200E

end 0x2000
