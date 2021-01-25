#ifndef SAL8_SLOT_H_GUARD
#define SAL8_SLOT_H_GUARD

#include <stdint.h>
#include <stddef.h>

typedef uint8_t SAL8Slot;

typedef struct {
    SAL8Slot* base;
    SAL8Slot* ptr;
    SAL8Slot* top;
} SAL8Stack;

void     sal8_stack_init(SAL8Stack* stk, uint8_t size);
void     sal8_stack_push(SAL8Stack* stk, SAL8Slot sl);
SAL8Slot sal8_stack_pop(SAL8Stack* stk);
void     sal8_stack_reset(SAL8Stack* stk);
void     sal8_stack_delete(SAL8Stack* stk);

#endif
