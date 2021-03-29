#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>

#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <drivers/uart.h>
#include <crypto/cipher.h>
#include <crypto/cipher_structs.h>
#include <tinycrypt/cbc_mode.h>
#include <tinycrypt/constants.h>
#include <logging/log.h>
#include <pthread.h>
#include <posix/mqueue.h>
#include <posix/posix_types.h>
#include <time.h>


#define Number_of_threads 3
#define UART_NAME "UART_0"
#define q_max_msgs 20
#define q_align 1
#define AES_KEY_LEN 16
#define AES_IV_LEN 16
void init_threads(pthread_t* thread_id);
void* uart_in_thread(void* x);
void* uart_out_thread(void* x);
void* process_thread(void* x);
int put_message_in_uart_queue(unsigned char* str);
int put_message_in_crypto_queue(unsigned char* str);
int validate_hw_compatibility(const struct device *dev);
void cbc_mode();

enum state{ 
    INIT, 
    ALIVE, 
    AVAIL, 
    KEY, 
    IV, 
    DECRYPT, 
    DLEN, 
    DATA, 
    SELECT_OPERATION, 
};
enum op{
    OP_KEY,
    OP_IV,
    OP_DECRYPT,
};