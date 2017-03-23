bits 32

mov eax, 4
mov ebx, 1
mov ecx, esp
add ecx, 0x1b
mov edx, 32
int 0x80

mov eax, 1
mov ebx, 0
int 0x80
