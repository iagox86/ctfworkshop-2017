bits 32

xor eax, eax
mov al, 4
xor ebx, ebx
inc ebx
mov ecx, 0x41414141
xor edx, edx
mov dl, 32
int 0x80

xor eax, eax
inc eax
xor ebx, ebx
int 0x80
