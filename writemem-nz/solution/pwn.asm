bits 32

mov eax, 0x41414141
xor ebx, ebx
mov bl, 1
mov dword [eax], ebx
ret
