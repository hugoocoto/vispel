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
Value env_add(char *name, Value value);
Value env_get(char *name);
Value env_set(char *name, Value value);

/* Closure stuff */
struct Env *get_current_env();
Value env_add_e(struct Env *e, char *name, Value value);
Value env_get_e(struct Env *e, char *name);
Value env_set_e(struct Env *e, char *name, Value value);
/* Create a new env and link with UPPER. Old current env is returned */
Env *env_create_e(Env *upper);
/* Destroy current env and set current env to CURRENT */
void env_destroy_e(Env *current);

#include <setjmp.h>
/* jmp here on error */
extern jmp_buf eval_runtime_error;

#endif // !ENV_H
