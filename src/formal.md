expr -> equality
equality -> comparison (( "!=" | "==") comparison)*
comparison -> term ((">" | ">=" | "<" | "<=" ) term)*
term -> factor (( "-" | "+" ) factor )*
factor -> unary (("/" | "\*" ) unary)*
unary -> ("!" | "-") unary | primary
primary -> NUM | STR | CHAR | "true" | "false"
