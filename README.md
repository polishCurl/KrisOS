# KrisOS
A simple embedded operating system with real-time focus. This is my Third Year Individual Project developed at the University of Manchester. KrisOS and the attached demo programs were developed for the Tiva C launchpad board with a Cortex-M4f based TM4C123GH6PM MCU.

### Main features
- A preemptive priority scheduler with time-slice preemption
- Heap manager
- Mutual exclusion locks with priority inheritance
- Semaphores
- Queues
- OS usage statistics task showing useful performance and debug data

### KrisOS - a user friendly operating system
- A single header file to include
- All configuration options in one place
- All features except for the scheduler are optional. Disable them to make KrisOS more compact
- Extensive code documentation
