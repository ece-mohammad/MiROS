/******************************************************************************
 * @file    miros.c
 * @brief   MiROS (Minimal Real-time Operating System) for embedded systems
 * @author  Mohammad Mohsen
 * @date    2023/08/24
 * @version 0.1.0
 * @license MIT License
 *          Copyright 2023, Mohammad Mohsen
 *          Permission is hereby granted, free of charge, to any person
 *          obtaining a copy of this software and associated documentation files
 *          (the “Software”), to deal in the Software without restriction,
 *          including without limitation the rights to use, copy, modify, merge,
 *          publish, distribute, sublicense, and/or sell copies of the Software,
 *          and to permit persons to whom the Software is furnished to do so,
 *          subject to the following conditions:
 *          The above copyright notice and this permission notice shall be
 *          included in all copies or substantial portions of the Software.
 *          THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 *          EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *          MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *          NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 *          BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 *          ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *          CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *          SOFTWARE.
 *******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "stm32f1xx_hal.h"
#include "miros.h"

#define MIROS_STACK_ALIGNMENT       8
#define MIROS_STACK_ALIGN_MASK      ((uint32_t)~(MIROS_STACK_ALIGNMENT - 1))
#define MIROS_EXCEPTION_RETURN      0xFFFFFFF9
#define MIROS_DEFAULT_PSR           0x21000000

#define MIROS_PEND_SVCall()         \
  do{  \
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;  \
  }while(0)

static Task_t *Miros_TaskQueue[MIROS_NUM_TASKS] = { 0 };
static uint32_t Miros_AddedTasks = 0;    // tail
static uint32_t MIROS_CurrentTaskIndex = 0; // head

static Task_t *Miros_RunningTask = NULL;
static Task_t *Miros_NextTask = NULL;

void MIROS_Initialize(void) {
  /*  intilize task queue  */
  for (uint32_t task_index = 0; task_index < MIROS_NUM_TASKS; task_index++) {
    Miros_TaskQueue[task_index] = NULL;
  }
  Miros_AddedTasks = 0;

  /*  initlaize task scheduler  */
  Miros_RunningTask = NULL;
  Miros_NextTask = NULL;
}

void MIROS_TaskInitialize(Task_t *task, TaskHandle_t handle, uint32_t *stack,
    uint32_t stack_size) {

  /* check that number of added tasks is less than MIROS_NUM_TASKS */
  assert_param(Miros_AddedTasks < MIROS_NUM_TASKS);

  uint32_t *sp;

  /**
   * ARM cortex requires stack to be aligned to word or double word addresses,
   * to make use of efficient data transfer instructions.
   * So, task's stack start & end addresses are aligned to the controller's
   * stack alignment, specified by `MIROS_STACK_ALIGNMENT` macro.
   *
   * stack in ARM cortex M3 grows towards lower addresses (decrementing stack).
   * sp (stack pointer) is assigned the end of allocated memory block for
   * the task's stack, than aligned to the
   * The start of the memory block is also aligned to the current address,
   * or the next aligned address.
   * */

  /*    align stack start   */
  sp = (stack + stack_size);
  sp = (uint32_t*) ((uint32_t) sp & MIROS_STACK_ALIGN_MASK);

  /*    align stack end     */
  stack = (uint32_t*) ((((uint32_t) stack - 1) & MIROS_STACK_ALIGN_MASK)
      + MIROS_STACK_ALIGNMENT);

  stack_size = (uint32_t) (sp - stack);

  /*    initialize task stack content   */
  *(--sp) = MIROS_DEFAULT_PSR; /* PSR */
  *(--sp) = (uint32_t) handle; /* PC  */
  *(--sp) = 0x11111111; /* LR  */
  *(--sp) = 0x12011012; /* R12 */
  *(--sp) = 0x03011030; /* R3  */
  *(--sp) = 0x02011020; /* R2  */
  *(--sp) = 0x01011010; /* R1  */
  *(--sp) = 0xDEADB00F; /* R0  */
  --sp;
  *(sp) = (uint32_t) sp + 1; /* R7 */
  *(--sp) = 0xDEADB44F; /* R4  */
  *(--sp) = 0xDEADB55F; /* R5  */
  *(--sp) = 0xDEADB66F; /* R6  */
  *(--sp) = 0xDEADB88F; /* R8  */
  *(--sp) = 0xDEADB99F; /* R9  */
  *(--sp) = 0xDEADBAAF; /* R10 */
  *(--sp) = 0xDEADBBBF; /* R11 */

  for (uint32_t *mem = sp; mem > stack;) {
    *(--mem) = 0xDEADBEEF;
  }

  /*    initialize stack structure  */
  task->handle = handle;
  task->stack = stack;
  task->stack_size = stack_size;
  task->stack_ptr = (uint32_t) sp;

  /*    add task from task queue & update number of added tasks    */
  Miros_TaskQueue[Miros_AddedTasks] = task;
  Miros_AddedTasks++;
}

void MIROS_Sched(void) {

  assert_param(Miros_AddedTasks > 0);

  /*    get task from task queue    */
  Miros_NextTask = Miros_TaskQueue[MIROS_CurrentTaskIndex];
  MIROS_CurrentTaskIndex++;
  if (MIROS_CurrentTaskIndex == Miros_AddedTasks) {
    MIROS_CurrentTaskIndex = 0;
  }

  MIROS_PEND_SVCall();
}

void HAL_SYSTICK_Callback(void) {
  MIROS_Sched();
}

void PendSV_Handler(void) {
  /* switch out current task    */

  if (Miros_RunningTask != NULL) {
    // save GPR registers
    __asm(
        "PUSH {R4}\n\t"
        "PUSH {R5}\n\t"
        "PUSH {R6}\n\t"
        "PUSH {R8}\n\t"
        "PUSH {R9}\n\t"
        "PUSH {R10}\n\t"
        "PUSH {R11}\n\t"
    );

    // save SP
    __asm (
        "MOV  R3, %0\n\t"
        "STR  SP, [R3, #8]\n\t"
        :
        : "r" (Miros_RunningTask)
    );
  }

  /* switch in next task    */

  /**
   * If at least 1 task is added, Miros_NextTask will never be NULL.
   * But, if, for some reason, n tasks were added, this will guard
   * against accessing a NULL pointer.
   * */
  if (Miros_NextTask != NULL) {
    Miros_RunningTask = Miros_NextTask;

    // load SP
    __asm (
        "MOV R3, %0\n\t"
        "LDR SP, [R3, #8]\n\t"
        :
        : "r" (Miros_NextTask)
    );

    // load GPR registers
    __asm(
        "POP {R11}\n\t"
        "POP {R10}\n\t"
        "POP {R9}\n\t"
        "POP {R8}\n\t"
        "POP {R6}\n\t"
        "POP {R5}\n\t"
        "POP {R4}\n\t"
    );

    // load R7
    /**
     * This step is necessary for GNU ARM GCC, as it uses R7 as a pointer to the
     * top of the current stack frame.
     *
     * At function entry, it pushes R7 into the stack, then allocates stack for
     * the function. Then copies SP into R7, then decrement R7 by the amount of
     * required stack allocations for the function. Then uses R7 to access the
     * stack (all allocated variables are located at offsets from R7).
     *
     * So at function return, R7 is incremented by the number of allocated stack
     * entries, then R7 is copied to SP, and the old value of R7 is popped from
     * the stack.
     *
     * So we copy SP to R7 as the last step before function exit, to keep SP
     * value unchanged.
     *
     * This works because, this function doesn't use any stack allocations
     * at all (no local variables, or function calls)
     * */
    __asm ("MOV R7, sp\n\t");
  }
}
