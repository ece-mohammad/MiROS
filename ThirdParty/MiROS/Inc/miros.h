/******************************************************************************
 * @file    miros.h
 * @brief   MiROS (Minimal Realtime Operating System) for embedded systems
 * @author  Mohammad Mohsen
 * @date    2023/08/24
 * @version 0.1.0
 * @license MIT license
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
 ******************************************************************************/

#ifndef _MIROS_H_
#define _MIROS_H_

/**
 * @brief The maximum number of allowed tasks
 * */
#define MIROS_NUM_TASKS             32

/**
 * @brief Task handle (function) typedef. The task handle is the address
 * of a normal C function, that is called when the task is ready to be executed.
 * MiROS task handles must be in he form of an infinite loop, and never to return.
 * */
typedef void (*TaskHandle_t)(void);

/**
 * @brief Task structure, that holds task's information
 *
 * uint32_t * stack: pointer to the task's allocated stack. The task's stack
 *    can be allocated in any manner the application deems fit.
 *    However, the stack must be allocated either statically (static local,
 *    static global or global scopes), or dynamically (using dynamic memory
 *    allocation). But never locally (as an automatic variable)
 *    It's recommended that the stack is double word aligned,
 *    as MiROS will align the stack to double word. This might result in
 *    some locations in the allocated stack not being utilized by MiROS.
 * uint32_t stack_size: Number ogf allocated memory **words** (not bytes)
 *    for the stack.
 * uint32_t stack_ptr: Current task's stack pointer (top of stack), indicates
 *    how much stack is currently used by the task.
 * TaskHandle_t handle: The task's handle, or function that will be called
 *    to run when the task is ready. It must be in the form of an infinite loop
 *    and never return.
 *
 * > MiROS keeps added tasks in a FIFO task queue, that means
 * > tasks that are added first, are scheduled first.
 *
 * > The task queue keeps pointers to the added tasks' structures. That
 * > means MiROS requires the tasks' structures to be allocated either
 * > statically, or dynamically, but never locally.
 * */
typedef struct {
  uint32_t *stack;
  uint32_t stack_size;
  uint32_t stack_ptr;
  TaskHandle_t handle;
} Task_t;

/**
 * @brief Initialize MiROS. Clears MiROS task queue and Initializes its
 * internal variables. Must be called before adding any tasks, and before
 * any interrupts are enabled.
 *
 * @param void
 * @return void
 * */
void MIROS_Initialize(void);

/**
 * @brief Initialize MiROS task structure, and adds the task to MiROS task
 * queue.
 *
 * @pre #MIROS_Initialized() was called
 * @pre Number of previously added tasks is less than #MIROS_NUM_TASKS
 *
 * @post @p task is added to MiROS task queue, in the next empty location in
 *    the task queue.
 * @post @p task will be scheduled to execute after all previously added tasks are
 *    scheduled.
 *
 * @param [in] task pointer to the task structure to be initialized
 * @param [in] handle address to the task's function
 * @param [in] stack pointer to the stack's allocated stack memory
 * @param [in] stack_size size of the allocated task's stack emmory
 * */
void MIROS_TaskInitialize(Task_t *task, TaskHandle_t handle, uint32_t *stack,
    uint32_t stack_size);

/**
 * @param Start MiROS RTOS scheduler, which selects the next ready task
 * to be executed.
 *
 * @pre #MIROS_Initialize() was called
 * @pre At least 1 task was added
 *
 * @post A task is seleced, and is switched in to be executed
 *    for 1 OS tick duration
 *
 * @param void
 * @param void
 * */
void MIROS_Sched(void);

#endif /* MIROS_H_ */
