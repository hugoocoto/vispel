#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "env.h"
#include "interpreter.h"
#include "tokens.h"

Env *lower_env = NULL;

static char *
gen_env_random_name()
{
        static int counter = 0;
        char name[12];
        name[snprintf(name, 11, "Env %d", counter)] = 0;
        counter++;
        return strdup(name);
}

static void
print_env_list()
{
        printf("\e[90m");
        Env *e = lower_env;
        while (e) {
                if (e != lower_env) printf(", ");
                printf("%s", e->name);
                e = e->upper;
        }
        printf("\e[0m");
        printf("\n");
}

struct Env *
get_current_env()
{
        return lower_env;
}


Value
env_add_e(struct Env *e, char *name, Value value)
{
        if (!e) {
                report("no env created!\n");
                longjmp(eval_runtime_error, 1);
        }
        if (shgeti(e->map, name) >= 0) {
                report("Var `%s` already declared\n", name);
                longjmp(eval_runtime_error, 1);
        }

        return shput(e->map, name, value);
}

Value
env_get_e(struct Env *env, char *name)
{
        if (!env) {
                report("no env created!\n");
                longjmp(eval_runtime_error, 1);
        }

        Env *e = env;
        node *ret;
        while (e && (ret = shgetp_null(e->map, name)) == NULL) {
                e = e->upper;
        }
        if (ret == NULL) {
                report("Var `%s` not declared\n", name);
                // longjmp(eval_runtime_error, 1);
                env_add(name, NO_VALUE);
                return (Value) NO_VALUE;
        }
        return ret->value;
}

Value
env_set_e(struct Env *env, char *name, Value value)
{
        if (!env) {
                report("no env created!\n");
                longjmp(eval_runtime_error, 1);
        }

        Env *e = env;
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

Env *
new_env()
{
        Env *e = calloc(1, sizeof(Env));
        e->name = gen_env_random_name();
        return e;
}

/* Create a new env and link with UPPER. Old current env is returned */
Env *
env_create_e(Env *upper)
{
        Env *ret = lower_env;
        Env *e = new_env();
        if (upper != NULL) {
                e->upper = upper;
        }
        lower_env = e;
        return ret;
}

/* Destroy current env and set current env to CURRENT */
void
env_destroy_e(Env *current)
{
        if (!lower_env) {
                report("Destroying a non existing env!\n");
                longjmp(eval_runtime_error, 1);
        }
        lower_env = current;
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
                longjmp(eval_runtime_error, 1);
        }
        lower_env = lower_env->upper;
}

Value
env_add(char *name, Value value)
{
        return env_add_e(lower_env, name, value);
}

Value
env_get(char *name)
{
        return env_get_e(lower_env, name);
}

Value
env_set(char *name, Value value)
{
        return env_set_e(lower_env, name, value);
}
