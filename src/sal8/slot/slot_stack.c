#include "slot.h"

#include "../mem/mem.h"

void sal8_stack_init(SAL8Stack* stk, uint8_t cap) {
    stk->base = SAL8_MEM_ALLOC(sizeof(SAL8Slot) * cap);
    stk->ptr = stk->base;
    stk->top = stk->ptr + cap;
}

void sal8_stack_push(SAL8Stack* stk, SAL8Slot sl) {
    *(stk->ptr++) = sl;
}

SAL8Slot sal8_stack_pop(SAL8Stack* stk) {
    return *((stk->ptr--) - 1);
}

uint8_t sal8_stack_size(SAL8Stack* stk) {
    return (uint8_t) (stk->ptr - stk->base);
}

uint8_t sal8_stack_capacity(SAL8Stack* stk) {
    return (uint8_t) (stk->top - stk->base);
}

SAL8Slot sal8_stack_get(SAL8Stack* stk, uint8_t index) {
    return *(stk->base + index);
}

void sal8_stack_reset(SAL8Stack* stk) {
    stk->ptr = stk->base;
}

void sal8_stack_delete(SAL8Stack* stk) {
    SAL8_MEM_FREE(stk->base);
}