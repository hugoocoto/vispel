#ifndef ENV_H
#define ENV_H

#include "tokens.h"

void env_add(vtok *id, Expr *value);
Expr *env_get(vtok *id);

#endif // !ENV_H
