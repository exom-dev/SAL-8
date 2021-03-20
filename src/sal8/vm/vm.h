#ifndef SAL8_VM_H_GUARD
#define SAL8_VM_H_GUARD

#include "../api.h"
#include "../io/io.h"
#include "../slot/slot.h"
#include "../cluster/cluster.h"

#include <stdint.h>
#include <stdbool.h>

#define SAL8_VM_RUNALL 0
#define SAL8_VM_CMP_EQUAL 0
#define SAL8_VM_CMP_BELOW 1
#define SAL8_VM_CMP_ABOVE 2

typedef struct {
    SAL8IO io;

    uint8_t regc;

    SAL8Slot* regs;
    SAL8Stack stk;

    SAL8Slot cmp;

    SAL8Cluster cl;
    uint8_t* ip;
} SAL8VM;

typedef enum {
    SAL8_VM_OK,
    SAL8_VM_ERROR
} SAL8VMStatus;

SAL8_API void         sal8_vm_init(SAL8VM* vm, uint8_t regCount, uint8_t stkCap);
SAL8_API void         sal8_vm_delete(SAL8VM* vm);
SAL8_API SAL8Slot     sal8_vm_access_register(SAL8VM* vm, uint8_t index);
SAL8_API uint32_t     sal8_vm_current_instruction_index(SAL8VM* vm);
SAL8_API void         sal8_vm_load(SAL8VM* vm, SAL8Cluster cl);
SAL8_API SAL8VMStatus sal8_vm_run(SAL8VM* vm, uint32_t count);
SAL8_API bool         sal8_vm_finished(SAL8VM* vm);

#endif
