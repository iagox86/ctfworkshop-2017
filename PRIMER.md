# Helpful hints

This document is an overview of assembly and shellcode, designed for the SkullSpace CTF Workshop. It'll cover tools, techniques, and instructions, and only assumes a development background.

You can find more info [on a tutorial I wrote](https://wiki.skullsecurity.org/index.php?title=Assembly), as well as in the [seminal guide to x86](https://software.intel.com/en-us/articles/intel-sdm), particularly Volume 2.

## Assembly + Machine code

The entire goal is to teach assembly and machine code, so let's quickly define what they are!

Assembly code is user-readable code. We'll see a lot today.

Machine code is the assembly code after it's been assembled.

Each assembly instruction has exactly one machine code to represent it. `nop` in assembly is ALWAYS\* `\x90` in machine code. And `\x90` is ALWAYS\* `nop`. That means that code can be losslessly assembled and disassmbled, unlike compiling.

\* It's not really important, but there is a tiny amount of meaningless ambiguity. `\x90` can actually disassemble to `xchg eax, eax`, which does nothing.

## Shellcode

Shellcode is code that an exploit tricks a program into running. It's typically short, self-contained, and free of NUL bytes.

The most important part of shellcode is: you're writing assembly/machine code has to be run completely self contained! No libraries, no .data section, nothing like that. All you have is code. That actually makes it a whole lot simpler!

If you need a string, the typical method is:

```
call get_string
  db 'this is the string', 0
get_string:
pop eax
```

The call pushes the return address onto the stack, which is the string. The pop moves the address into eax.

But let's not get ahead of ourselves!

### NUL (\x00) bytes

Just a quick comment: shellcode frequently avoids NUL (\x00) bytes. The reason is, functions like `strcpy()`, `strcat()`, etc. truncate strings when they hit a NUL, so shellcode doesn't work.

There are tricks to avoid NUL bytes in your code, for example, `mov eax, 0` becomes `xor eax eax`.

Many of the exercises, the ones with -nz on the end of the names, don't allow NUL bytes. It's up to you to change your shellcode accordingly!

### Raw code vs. ELF/PE

To make a long story short, when you compile a program with `gcc` or other tools, the raw code is compiled into an ELF file (on Linux), or a PE file (on Windows). Or several other executable formats. Those contain a section for data, a section for code, relocation information, libraries, etc etc. Enough that the OS knows how to run it.

When you compile with `nasm`, you don't have any of that. All you have is the raw instructions compiled to machine code. If you double click it or +x it, it won't work. The OS doesn't know what to do with it. you can use [tools/run_raw_code.c](tools/run_raw_code.c) to run it, but it won't run directly.

Shellcode doesn't need (and can't have) any of that extra information.

### Endianness

Endianness is something that's kinda hard to wrap your head around, but you're going to have to fairly early on in this workshop.

Human-readable values can be 1, 2, 4, or 8 bytes (typically). The way those are represented in memory are their "endianness". Endianness refers to whethre the first byte of the number is first, or if the firs byte is last.

For example, let's think of the number 0x1122. It's two bytes long - one byte is 0x11, and the other byte is 0x22.

On a **big endian** system, which is uncommon, that would be stored in memory as `11 22`. If you look at the memory before and after, you might see `00 00 00 00 00 11 22 00 00`.

On a **little endian** system, which is more common (i386, for example, is little endian), the bytes are stored with the least significant first - ie, 0x11223344 would be stored `44 33 22 11` in memory. That's what you'll pretty much always see, so you'll just have to get used to it.

If it helps, the reason is to make truncation easier. If you want to take the uint32 value 0x11223344, and cast it to a uint16, you'd expect it to be 0x3344. If you truncate 0x11223344 to a single byte, you'd expect it to be the last one - 0x44. When the value is stored as `44 33 22 11`, and you truncate it to a byte, the address doesn't change.

### ASLR

ASLR stands for Address Space Layout Randomization. It's a feature in most modern operating systems that makes exploitation harder. It means that memory addresses (for the stack, etc) change on every execution.

That means that challenges in this workshop that have an address will change every time you run it - you can't hardcode memory addresses!

### Self modification

Because shellcode is self contained, it can modify itself! Commonly, this is to get around character restrictions. If you need shellcode that's fully alpha-numeric, for example, you can't do a syscall (`int 0x80` => `\xcd\x80`). Therefore, you have to encode the syscall as two values that match the constraints, then xor them together (fortunately `xor eax, 0x41414141` => `"\x35\x41\x41\x41\x41"` => `"5AAAA"`).

That's super beyond the scope of this document, but there is one challenge - `b-64-b-tuff` - where you probably need to do that. Feel free to play with it. :)

## Assembly flavours

There are two flavours of i386/ia-64 assembly: Intel and AT&T.

Intel looks like this:
```
  400239:       6c                      ins    BYTE PTR es:[rdi],dx
  40023a:       69 62 36 34 2f 6c 64    imul   esp,DWORD PTR [rdx+0x36],0x646c2f34
```

AT&T looks like this:
```
  400239:       6c                      insb   (%dx),%es:(%rdi)
  40023a:       69 62 36 34 2f 6c 64    imul   $0x646c2f34,0x36(%rdx),%esp
```

Notice that the parameters are in a different order, the brackets are different, and registers have a `%` in front of them in AT&T.

Some people prefer AT&T syntax; they're wrong. We'll be using Intel throughout, and I'll show you how to configure the AT&T-defaulting tools to use Intel :)

## Tools you might need

### nasm / ndisasm

We'll be using nasm for the exercises. There are other assemblers, but nasm is nice and easy. And defaults to Intel.

You'll want to start every file with `bits 32` on the first line, followed by assembly.

You can assemble code with `nasm -o filename filename.asm`

You can disassemble code with `ndisasm -b32 filename` or `ndisasm -b32 - < filename.bin`. Change -b32 to -b64 for ia-64 binaries (most of our exercises are i386).

Note that, as discussed above, this won't compile the code in such a way that it'll run from a commandline - it'll compile directly to machine code. That means it has to be loaded into memory and run, there's no ELF or PE structure. You can run it with the [tools/run_raw_code.c](tools/run_raw_code.c) program that I'm including. Fun fact: it'll run equally well on Linux or Windows after being compiled, as long as you aren't doing any syscalls!

Similarly, `ndisasm` won't work for object files (programs that you compile with gcc). `objdump -D` can be used for those, or IDA/w32disasm.

### netcat

Netcat is used to connect to a host on a specific port, like telnet. It's very raw - it does nothing fancy, just sends and receives data.

You can do some really cool stuff with it (google "gender bending netcat" or "ed skoudis netcat" for tons of stuff), but our uses are simple: sending and receiving data.

To send data to a host, you can simply pipe it in:

`echo 'data' | nc -vv <host> <port>`

If you need to send binary, use `echo -ne`:

`echo -ne '\x00\x01\x02\x03' | nc -vv <host> <port>`

You can also write a program that outputs exploit code, and send that out:

`ruby ./generate_exploit.rb | nc -vv <host> <port>`

That's pretty much all you need for exercises! You're also welcome to use python/ruby sockets to complete them - and that might be required for later ones, if the data changes based on the connection (ie, you need to deal with a memory address).

If you don't have `nc` installed, try `ncat`. If you have neither of them, you'll probably want to install one (or do all your work through ruby/python sockets).

### objdump

`objdump` is a program for disassembling binaries (among other things). This handles ELF/PE files, unlike `ndisasm`, which does raw code.

It's super simple, but enough for what we're doing. If you want to get fancy, grab a copy of IDA or w32disasm instead.

The syntax you'll need is `objdump -M intel -D`:

```
$ objdump -M intel -D `which nc`

/usr/bin/nc:     file format elf64-x86-64


Disassembly of section .interp:

0000000000400238 <.interp>:
  400238:       2f                      (bad)
  400239:       6c                      ins    BYTE PTR es:[rdi],dx
  40023a:       69 62 36 34 2f 6c 64    imul   esp,DWORD PTR [rdx+0x36],0x646c2f34
```

### strace

`strace` prints out every syscall being made by the program. This is a great way to reverse engineer a program or debug shellcode:

```
$ strace nc -l -p 1234
...
socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
setsockopt(3, SOL_SOCKET, SO_REUSEADDR, [1], 4) = 0
bind(3, {sa_family=AF_INET, sin_port=htons(1234), sin_addr=inet_addr("0.0.0.0")}, 16) = 0
listen(3, 1)                            = 0
accept(3,
```

We can instantly see what kind of socket is created, how it's bound, that it's listening, and that it's waiting on accept. If we send it some data, we'll see that as well:

```
...
accept(3, {sa_family=AF_INET, sin_port=htons(36006), sin_addr=inet_addr("127.0.0.1")}, [16]) = 4
close(3)                                = 0
poll([{fd=4, events=POLLIN}, {fd=0, events=POLLIN}], 2, 4294967295) = 1 ([{fd=4, revents=POLLIN}])
read(4, "hellllo\n", 2048)              = 8
write(1, "hellllo\n", 8hellllo
)                = 8
```

Notice that it accepts the connection, gets socket 4, then closes socket 3 (the listener). Programs that accept multiple connections wouldn't do that, they'd leave socket 3 open - that tells us something we already know about `nc`, but if we're were reverse engineering it, that could be interesting. It then reads from socket 4 (the new socket), and writes to socket 1 (stdout).

Socket 0 is stdin, socket 1 is stdout, and socket 2 is stderr.

### gdb

`gdb` is crazy complex! I'll just mention a few things that are helpful for the exercises (a few things became a bunch of things, but hopefully it's helpful!). There is TONS more!!

Before using `gdb`, you'll probably want to enable Intel-style disassembly instead of AT&T:

`echo 'set disassembly-flavor intel' > ~/.gdbinit`

Then you can start `gdb` either by running a program (if you want to actively examine / change the state):

```
$ echo 'AAAA' > fake_code
$ gdb --args ./run-raw-code ./fake_code
...
(gdb) run
Starting program: run-raw-code ./fake_code
allocated 5 bytes of executable memory at: 0xf7fd5000

Program received signal SIGSEGV, Segmentation fault.
0xf7fd5fff in ?? ()
(gdb)
```

or you can enable core files and use it to examine a crash (very common if debugging a program changes something important, like the environment):

```
$ ulimit -c unlimited
$ ./run-raw-code ./fake_code
allocated 5 bytes of executable memory at: 0xf7fd5000
Segmentation fault (core dumped)

$ gdb ./run-raw-code ./core
...
Core was generated by `./run-raw-code ./fake_code'.
Program terminated with signal SIGSEGV, Segmentation fault.
#0  0xf7fd5fff in ?? ()
(gdb)
```

once you're in there, you can use `help` for instructions, but some of the more important ones are...

Examine a register:
```
(gdb) print/x $eax
$1 = 0xf7fd5041
```

Examine all registers:
```
(gdb) info reg
eax            0xf7fd5041       -134393791
ecx            0xf7fd5004       -134393852
edx            0x5      5
ebx            0x5      5
esp            0xffffc570       0xffffc570
ebp            0xffffc5f8       0xffffc5f8
esi            0x0      0
edi            0x0      0
eip            0xf7fd5fff       0xf7fd5fff
eflags         0x10a86  [ PF SF IF OF RF ]
cs             0x23     35
ss             0x2b     43
ds             0x2b     43
es             0x2b     43
fs             0x0      0
gs             0x63     99
```

Set a register:
```
(gdb) set $eax=1
(gdb) print/x $eax
$1 = 0x1
```

Examine the data a register points to:
```
(gdb) x/xw $esp
0xffffc570:     0x0000000a
```

...as a byte:
```
(gdb) x/xb $esp
0xffffc570:     0x0a
```

...as multiple bytes:
```
(gdb) x/16xb $esp
0xffffc570:     0x0a    0x00    0x00    0x00    0x00    0x50    0xfd    0xf7
0xffffc578:     0x05    0x00    0x00    0x00    0x21    0x00    0x00    0x00
```

...as a string (bytes followed by a NUL (`\x00`):
```
(gdb) x/xs $esp
0xffffc570:     "\n"
```

...etc. Look up the 'x' (eXamine) command for more info.

Look at the most recent value that was pushed to the stack:
```
(gdb) x/xw $esp
0xffffc570:     0x0000000a
```

Look at the most recent 16 values pushed onto the stack:
```
(gdb) x/16xw $esp
0xffffc570:     0x0000000a      0xf7fd5000      0x00000005      0x00000021
0xffffc580:     0xffffffff      0x00000000      0xffffc5a0      0x08048301
0xffffc590:     0xf7fd5000      0x0804b008      0x0000fc01      0x00000000
0xffffc5a0:     0xffff0000      0x00280ad4      0x000081a0      0x00000001
```

Inspect a memory address:
```
(gdb) x/xw 0x08048301
0x8048301:      0x696c5f5f
```

Print out the current instruction (where it crashed or where the breakpoint was):
```
(gdb) x/i $eip
=> 0xf7fd5fff:  add    BYTE PTR [eax+eiz*2-0x3],ah
```

Print out the instructions leading up to the current instruction (you just have to guess the length, but most instructions are 1-5 bytes long):
```
(gdb) x/8i $eip-10
   0xf7fd5ff5:  add    BYTE PTR [eax],al
   0xf7fd5ff7:  add    BYTE PTR [eax],al
   0xf7fd5ff9:  add    BYTE PTR [eax],al
   0xf7fd5ffb:  add    BYTE PTR [eax],al
   0xf7fd5ffd:  add    BYTE PTR [eax],al
=> 0xf7fd5fff:  add    BYTE PTR [eax+eiz*2-0x3],ah
   0xf7fd6003:  neg    DWORD PTR [ecx]
   0xf7fd6005:  pop    esp
```

### xinetd

Just a quick note: all the exercises use `xinetd` for listening. That means that stdin and stdout get mapped into the socket. For local testing, you can simply run the program, `echo EXPLOIT | ./program`, and look at stdin/stdout. When it's ready, you can just switch to a socket with netcat: `echo EXPLOIT | nc -vv <host> <port>`.

## Tools I wrote

I wrote a few tools to help you out with nasm's limitations. Find them in the [tools/ directory](tools/).

### assemble-to-stdout.rb

`ruby assemble-to-stdout.rb code.asm`

Asssembles the code and prints the binary machine code to stdout. Unfortunately, you can't do `nasm -o- file.asm`, so this solves that problem.

I frequently find it helpful to pipe output to `hexdump -C`:

```
$ ruby ./assemble-to-stdout.rb ./pwn.asm | hexdump -C
00000000  b8 05 00 00 00 e8 13 00  00 00 2f 68 6f 6d 65 2f  |........../home/|
00000010  63 74 66 2f 66 6c 61 67  2e 74 78 74 00 5b b9 00  |ctf/flag.txt.[..|
...
```

### binary-to-string.rb

You can pass this either .asm file (which will be assembled) or a .bin file (which will be used directly). It'll print it out in a way that can be used with `echo -ne` or in a C/Python/Ruby string.

For example:

```
$ ruby ./binary-to-string.rb ../readfile/solution/pwn.asm
\xb8\x05\x00\x00\x00\xe8\x13\x00\x00\x00\x2f\x68\x6f\x6d\x65\x2f\x63\x74\x66\x2f\x66\x6c\x61\x67\x2e\x74\x78...
```

From there, you can use it with `echo -ne` and `nc`:

`echo -ne "\xb8\x05\x00\x00\x00\xe8\x13\x00\x00\x00\x2f\x68\x6f\x6d\x65\x2f\x63\x74\x66\x2f\x66\x6c\x61\x67\x2e\x74\x78..." | nc <host> <port>`

### run_raw_code.c

This lets you test machine code by running it directly on the commandline:

```
$ ./run-raw-code ../readfile/solution/pwn
allocated 81 bytes of executable memory at: 0xf7fd5000

<garbage displayed here>
```

You can combine this with `strace` to debug shellcode:

```
$ strace ./run-raw-code ../readfile/solution/pwn
...
open("/home/ctf/flag.txt", O_RDONLY)    = -1 ENOENT (No such file or directory)
read(-2, 0xffffc560, 32)                = -1 EBADF (Bad file descriptor)
write(1, "\n\0\0\0\0P\375\367Q\0\0\0!\0\0\0\377\377\377\377\0\0\0\0\220\305\377\377\1\203\4\10", 32
```

(notice that the return from open() is -1 - the shellcode didn't work because the flag.txt file was missing!)

## Assembly reference

### Registers

A register is, quite simply, a variable that isn't stored in memory: it's stored in a special, magical place in the CPU. But basically, it's a variable that has to be used for most instructions (you can't reference memory directly). Different architectures (i386, ia-64, MIPS, SPARC, ARM, etc) have different numbers of registers, and different names.

We're going to focus on Intel i386 (aka, x86), which has about 9 useful registers. Some of them have implicit or explicit meanings, and some of them are general purpose.

They are:
* `eax`: Always used as a return value in all major compilers (when a function returns, the output is eax) - also used as a general purpose register everywhere else
* `ebx`: General purpose
* `ecx`: Sometimes used as a counter
* `edx`: General purpose
* `esi`: Sometimes used as a source for string-processing instructions; otherwise, general purpose
* `edi`: Sometimes used as a destination for string-processing instructions; otherwise, general purpose
* `ebp`: Frequently used as a "base pointer" (you don't need to know what that means :) ); can be general purpose
* `esp`: Always used as the stack pointer; used implicitly by a bunch of commands (like `call`)
* `eip`: Always used as a pointer to the current instruction; is special in many ways, can't be directly changed or read

There are also flags (`zf`, `cf`, etc), floating point registers, and other special stuff. I'm not going to cover any of that.

#### Sub-registers

The registers can also be broken down and referenced as such.

`eax`, for example, can be addresses as a 16-bit register, `ax`. Changing `ax` changes the lower 16 bits of `eax`.

`ax` can be further broken down into `ah` (for the first byte) and `al` (for the second). `al` is by far the most common variation of `eax`, since it lets you change the last byte.

A common way to set `eax` to, for example, 5, without using a NUL byte is:

```
xor eax, eax
mov al, 5
```

That sets al to `0x05`, ah to `0x00`, ax to `0x0005`, and eax to `0x00000005`.

To attempt to draw it out...

    31                             0
     +-----------------------------+
     |             eax             |
     +--------------+--------------+
     |              |      ax      |
     +--------------+------++------+
     |              |  ah  ||  al  |
     +--------------+------++------+

### Stack

The stack isn't *super* important for these exercises, so we're only going to cover the absolute basics. In the case of real exploitation, it's exceedingly important. I cover the stack in gory, gory detail in [an old blog post](https://blog.skullsecurity.org/2013/ropasaurusrex-a-primer-on-return-oriented-programming).

The stack is a chunk of memory that is used for temporary values. Values such as local variables, parameters to a function, return addresses, and saved registers. When a function is entered, it puts its own values on the stack. When it returns, the values stay on the stack, but are never accessed again (and are quickly overwritten). The stack memory that a function reserves for itself is called its 'stack frame'.

When something is pushed onto the stack, 4 is subtracted from `esp`, so it points at the "next" address, then the value is written. When something is popped from it, the current value is read, then 4 is added to `esp`. The value stays there, but values below `esp` are never accessed, and the next push will overwrite it.

If you want to see the most recently pushed value in `gdb`, use `x/xw $esp`. If you want to see the previous, it's `x/xw $esp+4`. And so on. Note that you'd use 8 on a 64-bit system.

Local values are written to the stack when they're declared or at the start of the function; it's up to the compiler. They're removed (esp is incremented) when the function ends, or when the compiler feels like it.

The `call` operation implicitly pushed the *return address* (ie, the address where the function should return) to the stack. The `ret` operation implicitly pops the value off the stack and jumps to it. That's the crux of stack exploitation, but we won't be covering that today.

This would be literally equivalent to `call`:

```
push return_addr
jmp some_function
return_addr:
```

We add 6, because the `push` instruction is (theoretically) 1 byte, and a jmp is 5 bytes (assuming it's longer than 0x80 bytes). Note that you can't add within an instruction like that.

This is equivalent to `ret` (except that it overwrites `eax`):

```
pop eax
jmp $eax
```

How parameters are passed and registers are saved is beyond the scope of the exercises.

### Instructions

This is going to be a quick tour of instructions that might come in handy...

* `mov dest, src` => `dest = src`
* `mov dest, dword [src]` => `dest = *src` (ie, the 32-bit value that src points to)
* `mov dest, dword [src+4]` => `dest = *(src+4)`
* `mov dword [dest], src` => `*dest = src`
* `mov dword [dest+4], src` => `*(dest+4) = src`
* `xor dest, src` => `dest = dest ^ src` (`and` and `or` work the same)
* `jmp addr` => Jump to the given address
* `call addr` => `push [return address] / jmp addr`
* `ret` => `pop [return address] / jmp [to it]`
* `inc reg / dec reg` => `reg++ / reg--`
* `cmp reg1, reg2` or `cmp reg1, [reg2]` => compare two values; usually followed by:
* `je addr / jz addr` => Jump if values are equal, or jump if the result is zero (means the same thing, and is represented by the same machine code)
* `jl addr / jle addr` => Jump if reg1 is less than reg2 / jump if reg1 is less than or equal to reg2
* `jg addr / jge addr` => Jump if greater than / greater than or equal
* `nop` => No operation (do nothing)
* `int xx` => Perform CPU interrupt xx (more info below)

A common way of cracking games that require a key is to find the comparison between the real key and the key the player entered and changing the `jz` to either a `jmp` or a `nop`, depending on whether you need it to always pass or always fail.

### Strings

As I mentioned above, shellcode has to be self contained. Now that we understand how the stack and call works, this should make more sense:

```
call get_string
  db 'this is the string', 0
get_string:
pop eax
```

This is seriously abusing the `call` instruction, because we aren't calling a function! Because we can't access `eip` directly, it's necessary to get it another way.

If you look at that code, the `call` line pushes the return address. The return address is immediately after a call, so it's the start of the string; a pointer to the 't'.

When it arrives at the get_string label, we do a `pop`. That `pop` pulls the return address back off the stack and stores it in `eax`. That means that `eax` is now a pointer to the return address, which is normally code, but in this case, it's our string.

### Syscalls

Let's talk about syscalls.

This is where Windows, Linux, and other i386-based operating systems differ. We're going to focus on Linux.

Linux syscalls are done by loading registers with the parameters, then running interrupt 128, or `int 0x80`.

I personally use [this reference](http://syscalls.kernelgrok.com/), although I find it helpful to memorize `sys_exit` (1), `sys_open` (5), `sys_read` (3), and `sys_write` (4). Most of my CTF exploits use those in one way or another.

For example, looking at the table at that link, syscall 1 is `sys_exit`. `eax` is always set to the syscall number, so 1. and `ebx` is always set to the `error_code`, let's say 0.

So we can write a simple program that just exits like:
```
mov eax, 1
mov ebx, 0
int 0x80
```

Here it is running:
```
$ echo -e 'bits 32\nmov eax, 1\nmov ebx, 0\nint 0x80\n' > exit.asm
$ nasm -o exit exit.asm
$ hexdump -C exit
00000000  b8 01 00 00 00 bb 00 00  00 00 cd 80              |............|
0000000c
$ ./run-raw-code exit
allocated 12 bytes of executable memory at: 0xf7fd5000
$
```

Looks about right! We can prove it's working by using a weird exit code, running it in strace:
```
$ echo -e 'bits 32\nmov eax, 1\nmov ebx, 555\nint 0x80\n' > exit.asm
$ nasm -o exit exit.asm
$ strace ./run-raw-code exit | tail
execve("./run-raw-code", ["./run-raw-code", "exit"], [/* 107 vars */]) = 0
...
open("exit", O_RDONLY)                  = 3
read(3, "\270\1\0\0\0\273+\2\0\0\315\200", 12) = 12
alarm(10)                               = 0
_exit(555)                              = ?
+++ exited with 43 +++
```

Sure enough!

The syscalls you'll probably want for exercises are the first few - `sys_open`, `sys_read`, and `sys_write`. Here's a quick working example of using `sys_write` to write a string to the terminal:

```
bits 32

mov eax, 4 ; 4 = sys_write
mov ebx, 1 ; 1 = stdout
call get_string
  db 'Hello World!', 0x0a
get_string:
pop ecx ; Get the string off the stack
mov edx, 13 ; length
int 0x80 ; syscall

mov eax, 1 ; sys_exit
mov ebx, 0 ; error_code
int 0x80 ; syscall
```

And we can confirm it works:

```
$ nasm -o test test.asm
$ make run-raw-code
gcc -o run-raw-code -m32 run-raw-code.c
$ ./run-raw-code ./test
allocated 48 bytes of executable memory at: 0xf7fd5000
Hello World!
```

### Debug tricks

There are two really helpful machine code commands for debugging:
* `int 0x03` (`"\xcc"`)
* `jmp $-2` (`"\xeb\xfe"`)

The first one is a debug breakpoint, and immediately exits / coredumps:
```
$ echo -ne '\xcc' > test
$ ./run-raw-code test
allocated 1 bytes of executable memory at: 0xf7fd5000
Trace/breakpoint trap (core dumped)
$
```

The second one is an infinite loop, and never exits:
```
$ ./run-raw-code test
allocated 2 bytes of executable memory at: 0xf7fd5000
```

...and it never ends (ctrl-c will kill it, of course).

When you have code working against your local instance and it doesn't work against the real service, try those two! If `\xcc` immediately kills the connection and `\xeb\xfe` hangs, you know you have working code and you should look at your shellcode. If both hang or both return immediately, your shellcode isn't running.
