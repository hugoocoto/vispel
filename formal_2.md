## EXPR
- expr -> assignment
- assignment -> IDENTIFIER "=" expr | andexpr
- andexpr -> orexpr "&&" andexpr | orexpr
- orexpr -> equality "||" orexpr | equality
- equality -> comparison (("!=" | "==") comparison)*
- comparison -> term ((">" | ">=" | "<" | "<=") term)*
- term -> factor (("-" | "+") factor)*
- factor -> unary (("/" | "\*") unary)*
- unary -> ("!" | "-") unary | call
- call -> group "(" expr? ("," expr)* ")"
- group -> "(" expr ")" | literal
- literal -> NUM | STR | "true" | "false" | IDENTIFIER

## STMT
- pogram -> declaration* EOF
- declaration -> vardecl | funcdecl | stmt
- vardecl -> "var" IDENTIFIER ("=" expr)? ";"
- funcdecl -> "fun" IDENTIFIER "(" expr? ("," expr)* ")" block
- stmt -> block | assert | exprstmt | if | while | return
- if -> "if" "(" expr ")" declaration ("else" declaration)?
- while -> "while" "(" expr ")" declaration
- return -> "return" expr ";"
- assert -> "assert" expr ";"
- exprstmt -> expr ";"
- block -> "{" (stmt ";")* "}"
