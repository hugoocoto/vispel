expr -> group
equality -> comparison (( "!=" | "==") comparison)*
comparison -> term ((">" | ">=" | "<" | "<=" ) term)*
term -> factor (( "-" | "+" ) factor )*
factor -> unary (("/" | "\*" ) unary)*
unary -> ("!" | "-") unary | group
group -> "(" expr ")" | primary
primary -> NUM | STR | CHAR | "true" | "false"
