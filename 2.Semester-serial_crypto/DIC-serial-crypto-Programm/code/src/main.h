#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <device.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <posix/time.h>
#include <drivers/uart.h>


#include <pthread.h>
#include <time.h>
// #include <uart.h>


#define Number_of_threads 3


void* uart_in_thread(void* x);
void* uart_out_thread(void* x);
void* process_thread(void* x);
