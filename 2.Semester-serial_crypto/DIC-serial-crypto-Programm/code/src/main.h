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
#include <posix/mqueue.h>
#include <posix/posix_types.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
// #include <uart.h>


#define Number_of_threads 3
#define UART_NAME "UART_0"

void init_threads(pthread_t* thread_id);
void* uart_in_thread(void* x);
void* uart_out_thread(void* x);
void* process_thread(void* x);
int put_message_in_uart_queue(unsigned char* str);
int put_message_in_crypto_queue(unsigned char* str);

enum state{ 
    INIT, ALIVE, AVAIL, KEY, IV, DECRYPT
};