/******************************************************************************
 * @file    round_robin.c
 * @brief   MiRound Robin scheduler with FIFO task queue
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

#include <stddef.h>
#include <stdint.h>

#include "main.h"
#include "miros.h"
#include "round_robin.h"

static Task_t *Sched_TaskQueue[MIROS_NUM_TASKS] = { 0 };
static uint32_t Sched_AddedTasks = 0;    // tail
static uint32_t Sched_CurrentTaskIndex = 0; // head

void Scheduler_Initialize(void) {
  /*  initialize task queue */
  for (uint32_t task_index = 0; task_index < MIROS_NUM_TASKS; task_index++) {
    Sched_TaskQueue[task_index] = NULL;
  }
  Sched_AddedTasks = 0;
}

void Scheduler_AddTask(Task_t *task) {

  assert_param(Sched_AddedTasks < MIROS_NUM_TASKS);

  /* add task from task queue & update number of added tasks */
  Sched_TaskQueue[Sched_AddedTasks] = task;
  Sched_AddedTasks++;
}

Task_t* Scheduler_GetTask(void) {
  Task_t * next_task = NULL;

  assert_param(Sched_AddedTasks > 0);

  next_task = Sched_TaskQueue[Sched_CurrentTaskIndex];

  /* get task from task queue */
  Sched_CurrentTaskIndex++;
  if (Sched_CurrentTaskIndex == Sched_AddedTasks) {
    Sched_CurrentTaskIndex = 0;
  }

  return next_task;
}
