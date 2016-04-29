#include "../include/semanic.h"

uint32_t hash_pjw(char* name)
{
    uint32_t val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASHSIZE) val = (val ^ (i >> 12)) & HASHSIZE;
    }
    return val;
}