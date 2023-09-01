# MiROS

Minimal Real-time Operating System, for embedded systems, with minimal features.
It is for educational purpose only, not tested, and as a result, is not fit for industrial purposes and comes with zero warranties.

## Features

- Round Robin scheduling algorithm
- Can support any number of tasks
- Minimalistic API, just 4 functions
- Separation between task management and scheduling

## Why

If you've seen (Modern Embedded Systems Programming Course by Quantum Leaps)[https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM], then you're probably familiar with the name. If not, I highly recommend it if you're interested in embedded systems.
`MiROS` is an educational real time operating system, made in the course and used as a tool to explain how real time operating systems kernels work. This version of `MiROS` is for a different micro-controller, and I've taken the liberty to add few things to it, like:
- This version of `MiROS` is compatible with `bluepill`'s controller `STM32F103Cxxx` from `STMicroelectronics`, while the version introduced in the course is compatible with `Tiva-C`'s controller `tm4c123gh6xx` from `Texas instruments`. However, this is not considered a port of `MiROS`, as I chose to write it from scratch for the learning experience. And I decided to share that with others, to help them have an easier time learning about RTOS kernels.

