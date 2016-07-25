; Test #4 Instructions which alter SR used in conditionals

org 0x1000
  alpha word  0x1234        ;1000
  beta  word  0xF000        ;1002

org 0x2000
  main
    add   &alpha, &beta     ;Sets the Carry bit, Z, N, V clear
    ifgt  word    0x1FC9    ;IFGT inst with 2 TC and 1 FC
;If greater than
    sub   &alpha, &beta     ;Executes this, SR bits remain the same
    rra   beta              ;No SR changes again
;Else
    mov #0, R3              ;This is not executed

end 0x2000
