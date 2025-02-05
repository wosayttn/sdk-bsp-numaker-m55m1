/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-25     tyx          first version
 */

.cpu cortex-m55
.syntax unified
.thumb
.text
.align 2

.global rt_secure_svc_handle

.global tzcall
tzcall:
    SVC     1                       /* call SVC 1 */
    BX      LR

.global tzcall_entry
tzcall_entry:
    PUSH    {R1, R4, LR}
    MOV     R4, R1                  /* copy thread SP to R4 */
    LDMFD   R4!, {r0 - r3}          /* pop user stack, get input arg0, arg1, arg2 */
    STMFD   R4!, {r0 - r3}          /* push stack, user stack recovery */
    BL      rt_secure_svc_handle    /* call fun */
    POP     {R1, R4, LR}
    STR     R0, [R1]                /* update return value */
    BX      LR                      /* return to thread */

syscall_entry:
    BX      LR                      /* return to user app */

.global SVC_Handler
SVC_Handler:
    MRS     R1, MSP                 /* get fault context from handler */
    TST     LR, #0x04               /* if(!EXC_RETURN[2]) */
    BEQ     _get_sp_done
    MRS     R1, PSP                 /* get fault context from thread */

_get_sp_done:

    LDR     R0, [R1, #24]
    LDRB    R0, [R0, #-2]

    CMP     R0, #0x0
    BEQ     syscall_entry

    CMP     R0, #0x1
    BEQ     tzcall_entry
