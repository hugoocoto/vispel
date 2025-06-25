#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

typedef struct node {
        char *key;
        Value value;
} node;

typedef struct Env {
        node *map;
        struct Env *upper;
} Env;

Env *lower_env = NULL;

Env *
new_env()
{
        return calloc(1, sizeof(Env));
}

void
env_create()
{
        Env *e = new_env();
        if (lower_env != NULL) {
                e->upper = lower_env;
        }
        lower_env = e;
}

void
env_destroy()
{
        if (!lower_env) {
                report("Destroying a non existing env!\n");
                exit(1);
        }
        lower_env = lower_env->upper;
}

Value
env_add(char *name, Value value)
{
        if (!lower_env) {
                report("no env created!\n");
                exit(1);
        }

        if (shgeti(lower_env->map, name) >= 0) {
                report("Var `%s` already declared\n", name);
                longjmp(eval_runtime_error, 1);
        }

        return shput(lower_env->map, name, value);
}

Value
env_get(char *name)
{
        if (!lower_env) {
                report("no env created!\n");
                exit(1);
        }

        Env *e = lower_env;
        node *ret;
        while (e && (ret = shgetp_null(e->map, name)) == NULL) {
                e = e->upper;
        }
        if (ret == NULL) {
                report("Var `%s` not declared\n", name);
                // longjmp(eval_runtime_error, 1);
                env_add(name, (Value) { .type = TYPE_STR, .str = "no-value" });
                return (Value) { .type = TYPE_STR, .str = "no-value" };
        }
        return ret->value;
}

Value
env_set(char *name, Value value)
{
        if (!lower_env) {
                report("no env created!\n");
                exit(1);
        }

        Env *e = lower_env;
        node *ret;
        while (e && (ret = shgetp_null(e->map, name)) == NULL) {
                e = e->upper;
        }
        if (ret == NULL) {
                report("Var %s not declared\n", name);
                longjmp(eval_runtime_error, 1);
        }
        return ret->value = value;
}
