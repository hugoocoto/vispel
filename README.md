# ViSPeL Specification

Very Simple Programming Language. The name is choosen because it's
memory unsafe and everything is a pointer.

## Types
- VAR: word size bytes container.
- MEM: memory address.

## Operators
- `(MEM) arg1, arg2, ...`: Call MEM with arguments arg1, arg2, ...
- `[MEM]->VAR`: Get the value at MEM.
- `+`, `-`, `/`, `*`: VAR_VAR->VAR or MEM_VAR->MEM

I dont know how to differenciate between MEM and VAR at the high level

