bits 32

mov eax, 4
mov ebx, 1
mov ecx, 0x41414141
mov edx, 32
int 0x80

mov eax, 1
mov ebx, 0
int 0x80
