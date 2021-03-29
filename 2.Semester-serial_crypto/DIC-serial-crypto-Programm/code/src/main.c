/*
	Author: Fabio Plunser

*/
#include "main.h"


//https://docs.zephyrproject.org/2.3.0/reference/kernel/data_passing/message_queues.html?highlight=queue
K_MSGQ_DEFINE(uart_queue, sizeof(char* ), q_max_msgs, q_align);
K_MSGQ_DEFINE(crypto_queue, sizeof(char* ), q_max_msgs, q_align);


const struct device * uart_dev;
const struct device * crypto_dev;
enum state st_state = INIT;
enum op operation;
bool processing_busy = false;  
//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#_CPPv411uart_config

//Random ciphertext key and plaintext from cbc sample
static uint8_t key_iv[16] ={
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
};
uint8_t * key = key_iv;
static uint8_t* cbc_buffer;
static uint8_t* out_buffer;
static uint32_t cap_flags;

static uint16_t len = 32;
void main(void)
{
	//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#api-reference
	//int uart_configure(structdevice *dev, conststructuart_config *cfg)
	uart_dev = device_get_binding(UART_NAME);
	if(!uart_dev){
		printk("UART-binding-error\n");
	}
	
	const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

	if(!uart_configure(uart_dev, &uart_cfg)){
		printk("UART-config-error\n");
	}

	
	pthread_t thread_id[Number_of_threads];
	init_threads(thread_id);
	while(1) {
		printk("Main-Thread is alive\n");
        sleep(5);
    }
	
}
void init_threads(pthread_t* thread_id)
{
	int i, thread_ok;
	void*(*threads[])(void*) = {uart_in_thread, uart_out_thread, process_thread};
	for(i=0; i<Number_of_threads; i++)
	{
		thread_ok = pthread_create(&thread_id[i], NULL, threads[i], NULL);
		if(thread_ok != 0)
		{
			printk("Thred creation Error\n");
		}
	}
	
}

void state_machine()
{
	// uint8_t uart_in = '\0';
	uint8_t i = 0;
	uint8_t uart_in;
	uint8_t* data_buffer = "";
	printk("In State Machine\n");
	while(1)
	{
		switch(st_state){
			case INIT: 
				// printk("Init State\n");
				if(!uart_poll_in(uart_dev, &uart_in)){
					printk("uart_in: %c\n", uart_in);
					switch(uart_in){
						case 'D':
						case 'd':
							if(processing_busy == false){
								st_state = DECRYPT;
							}
							
							break;
						case 'K':
						case 'k':
							if(processing_busy == false)
							{
								st_state = KEY;
							}
							break;
						case 'I':
						case 'i':
							if(processing_busy == false){
								st_state = IV;
							}
							break;
						case 'p':
						case 'P':
							put_message_in_crypto_queue("PROCESSING AVAILABLE\n");
							break;
						case '.':
							put_message_in_uart_queue(".\n");
							// if(processing_busy == false)
							// {
							// 	put_message_in_uart_queue("BUSY\n");
							// }
							break;
						default:
							break;
					}
				}
				else{
					// printk("Nothin received\n");
					break;
				}
				break;
			
			case DECRYPT:
				printk("DECRYPT\n");
				processing_busy = true;
				st_state = DLEN;
				operation = OP_DECRYPT; 
				break;
			case IV: 
				st_state = DATA;
				operation = OP_IV;
				data_buffer = (uint8_t*)malloc(AES_IV_LEN);
				len = AES_IV_LEN;
				break;
			
			case KEY: 
				st_state = DATA;
				operation = OP_KEY;
				data_buffer = (uint8_t*)malloc(AES_KEY_LEN);
				len = AES_KEY_LEN;
				break;

			case DLEN:
				st_state = DATA;
				i = 0;
				while(1)
				{
					if(!uart_poll_in(uart_dev, &uart_in)){ 
						len = uart_in; 
						break;
					}
				}
				data_buffer = (uint8_t*)malloc((len + AES_IV_LEN) * sizeof(uint8_t));
				memcpy(data_buffer, key_iv, AES_IV_LEN);
				data_buffer += AES_IV_LEN;

			case DATA:
				st_state = SELECT_OPERATION;
				i = 0;
				printk("DATA\n");
				while(len> i){
					if(!uart_poll_in(uart_dev, &uart_in)){ 
						printk("Data: 0x%02X\n", uart_in);
						data_buffer[i++] = uart_in;
					}
				}
				break;

			case SELECT_OPERATION:
				switch (operation)
				{
				case OP_KEY:
					printk("OP_IV\n");
					memcpy(key, data_buffer, AES_KEY_LEN);
					st_state = INIT; 
					break;
				case OP_IV:
					printk("OP_IV\n");
					memcpy(key_iv, data_buffer, AES_IV_LEN);
					st_state = INIT; 
					break;
				case OP_DECRYPT:
					printk("OP_DECRYPT\n");
					cbc_buffer=data_buffer;
					put_message_in_crypto_queue("D\n");
					st_state = INIT; 
					break;
				
				default:
					break;
				}
				break;
			default:
				break;

		}
	}
}
//execute state_maschine in thread
void* uart_in_thread(void * x){
	
	state_machine();
	
	return x;
}
//put string into uart queue
int put_message_in_uart_queue(unsigned char* str)
{	
	printk("Putting %s into uart_queue\n", str);
	if(k_msgq_put(&uart_queue, &str, K_FOREVER)!=0){
		printk("Couldn't put message in queue!!\n");
	}
	return 0;
}
//send uart messages from queue
void* uart_out_thread(void * x)
{
	int i=0;
	uint32_t len; 
	unsigned char* message;
	while(1)
	{
		if(!k_msgq_get(&uart_queue, &message, K_NO_WAIT)) {
			len = strlen(message);
			printk("Message in queue: %s, leng: %i\n", message, len);
			while(i < len)
			{
				uart_poll_out(uart_dev, message[i++]);
			}
			i = 0;
		}
	}
	return x;
}

//put string into queue
int put_message_in_crypto_queue(unsigned char* str)
{
	k_msgq_put(&crypto_queue, &str, K_FOREVER);
	return 0;
}



void cbc_mode()
{	
	printk("CBC_Mode: \n");
	printk("Key: %s\nIV: %s\nBuffer: %s\nBuffer 0x%02x\n", key, key_iv, cbc_buffer, cbc_buffer);
	uint32_t  in_buffer_len = len + AES_IV_LEN; 
	uint32_t out_buffer_len = len; 
	out_buffer = malloc(out_buffer_len);

	
	struct cipher_ctx ini = {
		.keylen = sizeof(key),
		.key.bit_stream = key,
		.flags = cap_flags,
	};
	struct cipher_pkt decrypt = {
		.in_buf = cbc_buffer,
		.in_len = in_buffer_len,
		.out_buf = out_buffer,
		.out_buf_max = out_buffer_len,
	};
	if(cipher_begin_session(crypto_dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CBC, CRYPTO_CIPHER_OP_DECRYPT)){
		cipher_free_session(crypto_dev, &ini);
	} 
	if (cipher_cbc_op(&ini, &decrypt, cbc_buffer)) {
		cipher_free_session(crypto_dev, &ini);
	}
}


//process encryption
void * process_thread(void * x) 
{
	unsigned char* message;
	while(1)
	{
		// printk("uart_out_thread_alive\n");
		if(!k_msgq_get(&crypto_queue, &message, K_NO_WAIT)) {
			
			switch (message[0])
			{
			case 'D':
				printk("Process_thread: Decrypting\n");
				processing_busy = true; 
				cbc_mode();
				break;
			
			default:
				break;
			}
		}
	}
	return x;
}


