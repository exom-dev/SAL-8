#include "slot.h"

#include "../mem/mem.h"

void sal8_stack_init(SAL8Stack* stk, uint8_t size) {
    stk->base = SAL8_MEM_ALLOC(sizeof(SAL8Slot) * size);
    stk->ptr = stk->base;
    stk->top = stk->ptr + size;
}

void sal8_stack_push(SAL8Stack* stk, SAL8Slot sl) {
    *(stk->ptr++) = sl;
}

SAL8Slot sal8_stack_pop(SAL8Stack* stk) {
    return *((stk->ptr--) - 1);
}

void sal8_stack_reset(SAL8Stack* stk) {
    stk->ptr = stk->base;
}

void sal8_stack_delete(SAL8Stack* stk) {
    SAL8_FREE(stk->base);
}