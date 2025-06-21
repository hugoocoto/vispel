expr -> assignment
assignment -> IDENTIFIER "=" expr | equality
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
stmt -> exprstmt | block
exprstmt -> expr ";"
block -> "{" (stmt ";")* "}"

