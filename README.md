# VSPL Specification

Very Simple Programming Language. This is another slow interpreter to compile
another useless language.

## showcase

```c
var c = 2;
func f(a) {
    a = a + c;
    println(a);
}
print("f(3) = ");
f(3);
```

## Compiler

This compiler is a generic implementation of a generic non-existing language.
The language rules are defined in [formal.md](./formal.md).

## Compiler Standard

The makefile compiles the compiler using gnu-c99 standard. It uses stb_ds as
the unique dependency.

## How to compile the compiler

Use the makefile to compile the compiler and run a test. If the test show any
output, it fails, so the compiler is not working for you. Sorry, it works in my
machine.
```sh
make
```

## Line Count
Just to say that I wrote a *2k line compiler!*.
[here](./wc.md)

## Turing Completeness
As I was able to implement a cellular automaton,
[rule110](https://en.wikipedia.org/wiki/Rule_110), that is known for being
[turing complete](https://en.wikipedia.org/wiki/Turing_completeness), I can say
that vspl is turing complete.

# I know 
I know I don't free a single byte, I would fix this some day. 
