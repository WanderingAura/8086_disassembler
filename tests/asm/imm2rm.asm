; imm to rm no disp
mov [bx+si], byte 123
mov [bx+di], byte -10

mov [bp+si], word -10000
mov [bx], word 32102

; imm to rm with disp

mov [bx+si+23], byte 123
mov [bx+di-23], word -10

mov [bp+10293], word -10000
mov [di-1203], word 32102