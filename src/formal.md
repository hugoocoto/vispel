expr -> assignment
assignment -> IDENTIFIER "=" expr | andexpr
andexpr -> orexpr "&&" andexpr | orexpr
orexpr -> equality "||" orexpr | equality
equality -> comparison (("!=" | "==") comparison)*
comparison -> term ((">" | ">=" | "<" | "<=") term)*
term -> factor (("-" | "+") factor)*
factor -> unary (("/" | "\*") unary)*
unary -> ("!" | "-") unary | group
group -> "(" expr ")" | literal
literal -> NUM | STR | CHAR | "true" | "false" | IDENTIFIER

pogram -> declaration* EOF
declaration -> vardecl | stmt
vardecl -> "var" IDENTIFIER ("=" expr)? ";"
stmt -> block | assert | exprstmt | ifstmt
ifstmt -> "if" "(" expr ")" declaration ("else" declaration)?
assert -> "assert" expr ";"
exprstmt -> expr ";"
block -> "{" (stmt ";")* "}"

