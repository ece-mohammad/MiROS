/******************************************************************************
 * @file    round_robin.h
 * @brief   Round Robin scheduler with FIFO task queue
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
#ifndef _INC_ROUND_ROBIN_H_
#define _INC_ROUND_ROBIN_H_

/**
 * @brief Initialize the scheduler
 *
 * @param void
 * @return void
 * */
void Scheduler_Initialize(void);

/**
 * @brief Add task to the scheduler's task queue
 *
 * @pre Scheduler is initialized by calling #Scheduler_Initialize()
 * @pre Task is initialized
 * @pre Number of preciously scheduled tasks is less than #MIROS_NUM_TASKS
 *
 * @post @p task is added to the scheduler's task queue and will be scheduled
 *   when all previously scheduled tasks are executed
 *
 * @param [in] pointer to the task to be added
 * */
void Scheduler_AddTask(Task_t *task);

/**
 * @brief Get next task to be executed
 *
 * @pre Scheduler is initialized by calling #Scheduler_Initialize()
 * @pre At least one task was added to the scheduler
 *
 * @param void
 *
 * @return Task_t *: pointer to the next task to be executed
 * */
Task_t* Scheduler_GetTask(void);

#endif /* _INC_ROUND_ROBIN_H_ */
