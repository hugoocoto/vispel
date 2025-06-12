# ViSPeL Specification

Very Simple Programming Language. The name is choosen because it's
memory unsafe and everything is a pointer.

## Types
- VAL: value inside a memory address.
- MEM: memory address.

## Operators
- `(MEM) arg1, arg2, ... ;`: Call MEM with arguments arg1, arg2, ...
- `[MEM]`: Get the value at MEM.
- `+`, `-`, `/`, `*`: VAR_VAR->VAR or MEM_VAR->MEM

I dont know how to differenciate between MEM and VAR at the high level

