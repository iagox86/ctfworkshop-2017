bits 32

xor eax, eax
mov al, 0x01
shl eax, 8
mov al, 0x02
shl eax, 8
mov al, 0x03
ret
