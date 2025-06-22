#ifndef ENV_H
#define ENV_H

#include "interpreter.h"
#include "tokens.h"

/* Create a new environment and set it as the lower one. For searches
 * and addings, start looking from lower to upper. This function should
 * be called on scope enter (as a new block). Destroy is the opposite. */
void env_create();
void env_destroy();

/* Add and get a variable. On error jump to eval_runtime_error */
void env_add(char *name, Value value);
Value env_get(char *name);
Value env_set(char *name, Value value);

#include <setjmp.h>
/* jmp here on error */
extern jmp_buf eval_runtime_error;

#endif // !ENV_H
