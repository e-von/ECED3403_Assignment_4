; Test #5 - Two conditionals in a row, the first of which has no counters set
; so all the conditionals are executed regardless. The second set its own COND
; and EX bits

org 0x1000
  alpha word  0x0001        ;1000
  beta  word  0x7FFF        ;1002

org 0x2000
  main
    add   &alpha, &beta     ;Sets the Carry bit, Z, N, V clear
    ifvs  word    0x1FC0    ;IFVS inst with 0 TC and 0 FC
;If V set
    sub   &alpha, &beta     ;Since there is no counter all instructions are
    rra   beta              ;executed
;Else
    mov #0, R3              ;Executed as well

    cmp   &alpha, &beta
    ifpl  word  0x1F55
;If unsigned higher
    jmp   exit
;Else
    mov #0, R3

org 0x2100
  exit

end 0x2000
