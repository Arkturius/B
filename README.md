# B

This is a repository for the post common-core 42 project 'B'.   
The goal is to build a compiler for the
[B programming language](https://www.nokia.com/bell-labs/about/dennis-m-ritchie/bref.html) using lexer/parser generators 
[lex](https://man7.org/linux/man-pages/man1/lex.1p.html)/
[flex](https://gothub.dev.projectsegfau.lt/westes/flex/)
and 
[yacc](https://www.man7.org/linux/man-pages/man1/yacc.1p.html)/
[bison](https://www.gnu.org/software/bison/).   
I went for the GNU versions *flex* and *bison* for this project.

There are a few rules about how the compiling stage should be done.   

- You must use syntax directed translation so your program must not use any form
of AST.
- The input language is B as defined by Thompson’s technical memo with the following additions/exceptions:

- Internal declaration (reference to a variable not declared as external or automatic) are treated as label declarations.
- Labels are in the same namespace as other symbols.
- Since B doesn’t have any type, there is no way to distinguish a function from
an int, this is why all function call are made by pointer and the symbol of a
function must designate a pointer to the function. For example, the function
f() return (42); could be defined as follow:
```asm
f:
.long f + 4
  enter   0, 0
  mov     eax, 42
  leave
  ret
```
- B was originally created for the PDP-11 computer which have word addressing
while i386 uses byte addressing, to address this problem you must translate
the expression a[b] as \*(a + 4 \* b) instead of \*(a + b)
- argc, argv and envp can be passed to the main in the same way as in C
- You can do character/string literal parsing as in C (with \ as escape char
instead of \*)
- There is a file brt0.o in attachment, this file contains the entrypoint for a b program
and a function syscall that work the same way has the c function syscall(2)
that will allow you to use syscalls in your B programs (using the extrn keyword to
declare it).

## Lexing

## Parsing

## Evaluation

## Code Generation

