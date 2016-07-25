; Rule #2 Test, True Condition, Execute Always False

org 0x1000
  alpha word  0x1234
  beta  word  0x2222

org 0x2000
  main
    add   &alpha, &beta     ;2000, 2002, 2004
    ifnv  word    0x1FF7    ;2006, IFNV inst with 1 TC and 3 FC
;If condition met
    rrc   beta              ;2008, 200A
;Else
    mov   &beta, &alpha     ;200C, 200E, 2010
    add   #1, &alpha        ;2012, 2014
    mov   #0, &beta         ;2016, 2018

end 0x2000
