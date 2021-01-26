#include "vm.h"
#include "../io/log.h"
#include "../cluster/bytecode.h"
#include "../mem/mem.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void sal8_vm_init(SAL8VM* vm, uint8_t regCount, uint8_t stkCap) {
    sal8_io_init(&vm->io);

    vm->regc = regCount;
    vm->regs = SAL8_MEM_ALLOC(sizeof(SAL8Slot) * regCount);

    sal8_stack_init(&vm->stk, stkCap);
}

void sal8_vm_delete(SAL8VM* vm) {
    SAL8_MEM_FREE(vm->regs);
    sal8_stack_delete(&vm->stk);
}

SAL8Slot sal8_vm_access_register(SAL8VM* vm, uint8_t index) {
    return vm->regs[index];
}

void sal8_vm_load(SAL8VM* vm, SAL8Cluster cl) {
    vm->cl = cl;
    vm->ip = cl.data;

    sal8_stack_reset(&vm->stk);

    for(uint8_t i = 0; i < vm->regc; ++i)
        vm->regs[i] = 0;
    for(uint8_t i = 0; i < sal8_stack_capacity(&vm->stk); ++i)
        vm->stk.base[i] = 0;

    vm->cmp = 0;
}

SAL8VMStatus sal8_vm_run(SAL8VM* vm, uint32_t count) {
    bool forever = (count == 0);
    uint8_t* limit = vm->cl.data + vm->cl.size;

    #define LEFT (*vm->ip)
    #define RIGHT (*(vm->ip + 1))

    #define LEFT_REG (vm->regs[LEFT])
    #define RIGHT_REG (vm->regs[RIGHT])

    #define LEFT_BY_TYPE (types & SAL8_BYTECODE_LEFT_TYPE_MASK ? LEFT_REG : LEFT)
    #define RIGHT_BY_TYPE (types & SAL8_BYTECODE_RIGHT_TYPE_MASK ? RIGHT_REG : RIGHT)

    #define IP_SET(offset) vm->ip = vm->cl.data + offset
    #define IP_SKIP(count) vm->ip += (count)
    #define IP_SKIP_BACKWARDS(count) vm->ip -= (count)

    #define VM_RUNTIME_ERROR(vm, fmt, ...) \
        SAL8_ERROR(vm->io, "[instruction %zu] " fmt "\n", (vm->ip - vm->cl.data) / SAL8_BYTECODE_FORMAT)

    while((forever || count > 0) && vm->ip < limit) {
        uint8_t opcode = (*vm->ip++);
        uint8_t types = opcode & SAL8_BYTECODE_TYPE_MASK;

        opcode &= SAL8_BYTECODE_OPCODE_MASK;

        switch(opcode) {
            case SAL8_OP_MOV: {
                LEFT_REG = RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_PUSH: {
                if(vm->stk.ptr == vm->stk.top) {
                    VM_RUNTIME_ERROR(vm, "Stack overflow");
                    return SAL8_VM_ERROR;
                }

                sal8_stack_push(&vm->stk, LEFT_BY_TYPE);

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_POP: {
                if(vm->stk.ptr == vm->stk.base) {
                    VM_RUNTIME_ERROR(vm, "Stack underflow");
                    return SAL8_VM_ERROR;
                }

                LEFT_REG = sal8_stack_pop(&vm->stk);

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_JMP: {
                IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                break;
            }
            case SAL8_OP_CMP: {
                uint8_t left = LEFT_BY_TYPE;
                uint8_t right = RIGHT_BY_TYPE;

                vm->cmp = (left == right ? SAL8_VM_CMP_EQUAL :
                          (left < right  ? SAL8_VM_CMP_BELOW :
                                           SAL8_VM_CMP_ABOVE));

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_JE: {
                if(vm->cmp == SAL8_VM_CMP_EQUAL)
                    IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                else IP_SKIP(SAL8_BYTECODE_FORMAT - 1);

                break;
            }
            case SAL8_OP_JA: {
                if(vm->cmp == SAL8_VM_CMP_ABOVE)
                    IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                else IP_SKIP(SAL8_BYTECODE_FORMAT - 1);

                break;
            }
            case SAL8_OP_JAE: {
                if(vm->cmp == SAL8_VM_CMP_ABOVE || vm->cmp == SAL8_VM_CMP_EQUAL)
                    IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                else IP_SKIP(SAL8_BYTECODE_FORMAT - 1);

                break;
            }
            case SAL8_OP_JB: {
                if(vm->cmp == SAL8_VM_CMP_BELOW)
                    IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                else IP_SKIP(SAL8_BYTECODE_FORMAT - 1);

                break;
            }
            case SAL8_OP_JBE: {
                if(vm->cmp == SAL8_VM_CMP_BELOW || vm->cmp == SAL8_VM_CMP_EQUAL)
                    IP_SET(LEFT * SAL8_BYTECODE_FORMAT);
                else IP_SKIP(SAL8_BYTECODE_FORMAT - 1);

                break;
            }
            case SAL8_OP_ADD: {
                LEFT_REG = LEFT_REG + RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_SUB: {
                LEFT_REG = LEFT_REG - RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_MUL: {
                LEFT_REG = LEFT_REG * RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_DIV: {
                uint8_t right = RIGHT_BY_TYPE;

                if(right == 0) {
                    VM_RUNTIME_ERROR(vm, "Cannot divide by zero");
                    return SAL8_VM_ERROR;
                }

                LEFT_REG = LEFT_REG / RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_MOD: {
                uint8_t right = RIGHT_BY_TYPE;

                if(right == 0) {
                    VM_RUNTIME_ERROR(vm, "Cannot mod by zero");
                    return SAL8_VM_ERROR;
                }

                LEFT_REG = LEFT_REG % RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_SHL: {
                LEFT_REG = LEFT_REG << RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_SHR: {
                LEFT_REG = LEFT_REG >> RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_OR: {
                LEFT_REG = LEFT_REG | RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_AND: {
                LEFT_REG = LEFT_REG & RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_XOR: {
                LEFT_REG = LEFT_REG | RIGHT_BY_TYPE;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_IN: {
                uint16_t input = SAL8_IN(vm->io);

                // There is no more valid input, so halt the VM.
                if(input > UINT8_MAX) {
                    IP_SKIP_BACKWARDS(1);
                    return SAL8_VM_OK;
                }

                LEFT_REG = input;

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            case SAL8_OP_OUT: {
                SAL8_OUT(vm->io, "%hhu\n", (uint8_t) LEFT_BY_TYPE);

                IP_SKIP(SAL8_BYTECODE_FORMAT - 1);
                break;
            }
            default: {
                VM_RUNTIME_ERROR(vm, "Illegal instruction");
                return SAL8_VM_ERROR;
            }
        }

        if(!forever)
            --count;
    }

    return SAL8_VM_OK;

    #undef VM_RUNTIME_ERROR

    #undef IP_SKIP_BACKWARDS
    #undef IP_SKIP

    #undef RIGHT_BY_TYPE
    #undef LEFT_BY_TYPE

    #undef RIGHT_REG
    #undef LEFT_REG

    #undef RIGHT
    #undef LEFT
}

bool sal8_vm_finished(SAL8VM* vm) {
    return vm->ip == (vm->cl.data + vm->cl.size);
}