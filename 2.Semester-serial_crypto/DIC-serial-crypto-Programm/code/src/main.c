// Author: FabioPlunser //
// Date: 10.3.2021 - 7.03.2021 //
// GIT-Repo: https://github.com/FabioPlunser/DIC-Lezuo
// Specific Git-location: https://github.com/FabioPlunser/DIC-Lezuo/tree/master/2.Semester-serial_crypto/DIC-serial-crypto-Programm //
// Compiled with make, in WSL using Ubuntu 20.0.4, as you can see in my Repo //

// DIC Project Crypto Processor //

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

#include "main.h"
//create uart device
const struct device * uart_dev;
//create crypto device
const struct device * crypto_dev;
//set standard enumeration state for state machine
enum state st_state = INIT;
//get enumeration
enum op operation;
//set busy flag false as standard
bool processing_busy = false;  

//https://docs.zephyrproject.org/2.3.0/reference/kernel/data_passing/message_queues.html?highlight=queue
//define two queues
K_MSGQ_DEFINE(uart_queue, sizeof(struct uart_message *), q_max_msgs, q_align);
K_MSGQ_DEFINE(crypto_queue, sizeof(char* ), q_max_msgs, q_align);



//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#_CPPv411uart_config

//set default ciphertext BBBBBBBBBBBBBBBB
static uint8_t iv[AES_IV_LEN] ={
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
	0x42,0x42,0x42,0x42,
};
//set default key, with care so that iv and key are not overwriting each other
uint8_t* key = iv;

static uint8_t* cbc_buffer;
static uint8_t* out_buffer;
static uint32_t cap_flags;

static uint16_t len;
void main(void)
{
	
	//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#api-reference
	//bind UART
	uart_dev = device_get_binding(UART_NAME);
	if(!uart_dev){
		printk("UART-binding-error\n");
	}
	//creat UART configuration
	const struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};	
	//set UART configuration
	if(!uart_configure(uart_dev, &uart_cfg)){
		printk("UART-config-error\n");
	}

	//bind crpyto
	crypto_dev = device_get_binding(CRYPTO_DRV_NAME);
	if (!crypto_dev) {
        printk("Crypto-binding-error\n");
        return;
    }
	//validate hardware for crypto device
	validate_hw_compatibility();

	//create thread array and initalize thread
	pthread_t thread_id[Number_of_threads];
	init_threads(thread_id);

	//send alive message
	while(1) {
		printk("Main-Thread is alive\n");
        sleep(5000);
    }
	
}
void init_threads(pthread_t* thread_id)
{	
	int i, thread_ok;
	//create function pointer array
	void*(*threads[])(void*) = {uart_in_thread, uart_out_thread, process_thread};
	//go through function pointer array and init threads
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
					printk("\033[1m\033[32mState Machine UART_IN: %c\033[0m\n", uart_in);
					switch(uart_in){
						case 'w':
						case 'W':
							put_message_in_crypto_queue("W\n");
							break;
						case 'D':
						case 'd':
							//doesn't make sense to go further if process thread is running
							//indented as provention of race conditions
							//because then the global cipertext is not able to change when process thread is running
							if(processing_busy == false){
								st_state = DECRYPT;
							}
							break;
						case 'K':
						case 'k':
							//doesn't make sense to go further if process thread is running
							//indented as provention of race conditions
							//because then the global KEY is not able to change when process thread is running
							if(processing_busy == false)
							{
								st_state = KEY;
							}
							break;
						case 'I':
						case 'i':
							//doesn't make sense to go further if process thread is running
							//indented as provention of race conditions
							//because then the global IV is not able to change when process thread is running
							if(processing_busy == false){
								st_state = IV;
							}
							break;
						case 'p':
						case 'P':
							put_message_in_crypto_queue("P\n");
							break;
						case '.':
							put_message_in_uart_queue(".\n", strlen(".\n"));
							if(processing_busy == true)
							{
								sleep(0.1); 
								//my programm sended busy to fast so i fixed it with a little sleep
								//I think the is that, the CPU usage of my CPU changes all the time and that has sometimes 
								//impact on how fast the threads are running. 
								put_message_in_uart_queue("BUSY\n", strlen("BUSY\n"));
							}
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
				st_state = DLEN;
				operation = OP_DECRYPT; 
				break;
			case IV: 
				st_state = DATA;
				operation = OP_IV;
				data_buffer = (uint8_t*)malloc(AES_IV_LEN); //allocate data_buffer for key transmission
				len = AES_IV_LEN;
				break;
			
			case KEY: 
				st_state = DATA;
				operation = OP_KEY;
				data_buffer = (uint8_t*)malloc(AES_KEY_LEN);//allocate data_buffer for key transmission
				len = AES_KEY_LEN;
				break;

			case DLEN:
				st_state = DATA;
				while(1)
				{
					if(!uart_poll_in(uart_dev, &uart_in)){ 
						len = uart_in; //get second transmission char that determines length of data
						break;
					}
				}
				data_buffer = (uint8_t*)malloc((len + AES_IV_LEN) * sizeof(uint8_t)); //allocate data_buffer for cipher text trasmission + IV
				memcpy(data_buffer, iv, AES_IV_LEN); //copy IV in first 16 char of buffer, crypto library needs this 
				data_buffer += AES_IV_LEN; //point pointer to 16th char so that ciphertext is after IV

			case DATA:
				st_state = SELECT_OPERATION;
				i = 0;	
				//recevie Data
				while(len> i){
					if(!uart_poll_in(uart_dev, &uart_in)){ 
						printk("\033[0;31m Data: 0x%02X\033[0m\n", uart_in);
						data_buffer[i++] = uart_in;
					}
				}
				break;

			case SELECT_OPERATION:
				switch (operation)
				{
				case OP_KEY:
					memcpy(key, data_buffer, AES_KEY_LEN); //copy data_buffer into global variable for use in process thread
					st_state = INIT; 
					break;
				case OP_IV:
					memcpy(iv, data_buffer, AES_IV_LEN); //copy data_buffer into global variable for use in process thread
					st_state = INIT; 
					break;
				case OP_DECRYPT:
					data_buffer -= AES_IV_LEN; //reset pointer of buffer to correctly copy into global varialbe
					cbc_buffer=data_buffer;
					put_message_in_crypto_queue("D\n"); //send decrypt command for process thread 
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
int put_message_in_uart_queue(unsigned char* str, uint32_t len)
{	
	//struct message for determing length, for correct sending zero termination 
	static struct uart_message message; 
	message.message = str; 
	message.len = len;
	struct uart_message * message_pointer = &message;
	
	printk("Putting into uart_queue: %s\n", str);
	if(k_msgq_put(&uart_queue, &message_pointer, K_FOREVER)!=0){
		printk("Couldn't put message in queue!!\n");
	}
	return 0;
}

//send uart messages from queue
void* uart_out_thread(void * x)
{
	int i=0;
	uint32_t len; 
	struct uart_message * message;
	unsigned char* message_temp;
	while(1)
	{
		if(!k_msgq_get(&uart_queue, &message, K_NO_WAIT)) {
			//get length and data from struct message
			len = message->len;
			message_temp = message->message; 
			printk("Message in queue: %s\n", message_temp);
			while(i < len)
			{
				uart_poll_out(uart_dev, message_temp[i++]);
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

void format_plaintext_for_comparison(uint8_t* plaintext)
{
	//formatting ciphertext for correct coparison in test.py
	uint8_t* plaintext_temp = malloc(strlen(plaintext)+3);
	plaintext_temp[0] = 'D'; plaintext_temp[1] = ' ';
	memcpy(plaintext_temp + 2, plaintext, strlen(plaintext));
	plaintext_temp[strlen(plaintext)+2] = '\x00';
	printk("Plaintext: %s\n", plaintext);
	put_message_in_uart_queue(plaintext_temp, strlen(plaintext_temp)+1);
}

//set flags for cbc mode
int validate_hw_compatibility()
{
	uint32_t flags = 0U;
	flags = cipher_query_hwcaps(crypto_dev);
    if ((flags & CAP_RAW_KEY) == 0U) {
            printk("Please provision the key separately "
                    "as the module doesnt support a raw key\n");
            return -1;
    }

    if ((flags & CAP_SYNC_OPS) == 0U) {
            printk("The app assumes sync semantics. "
              "Please rewrite the app accordingly before proceeding\n");
            return -1;
    }

    if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) {
            printk("The app assumes distinct IO buffers. "
            "Please rewrite the app accordingly before proceeding\n");
            return -1;
    }
	cap_flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;
	return 0;
}

int cbc_mode()
{	
	printk("CBC_Mode: \n");
	printk("Key: %s\nIV: %s\nBuffer: %s\nBuffer 0x%02x\n", key, iv, cbc_buffer, cbc_buffer);
	uint32_t  in_buffer_len = len + AES_IV_LEN; 
	uint32_t out_buffer_len = len; 
	out_buffer = malloc(out_buffer_len);

	struct cipher_ctx ini = {
		.keylen = AES_KEY_LEN,
		.key.bit_stream = key,
		.flags = cap_flags,
	};
	printk("Keylen: %i, keybitstream: %s , flags %i\n", ini.keylen, ini.key.bit_stream, ini.flags);
	struct cipher_pkt decrypt = {
		.in_buf = cbc_buffer,
		.in_len = in_buffer_len,
		.out_buf = out_buffer,
		.out_buf_max = out_buffer_len,
	};
	
	if(cipher_begin_session(crypto_dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CBC, CRYPTO_CIPHER_OP_DECRYPT)){
		cipher_free_session(crypto_dev, &ini); put_message_in_uart_queue("XERROR\n", strlen("XERROR\n")); return 0;
	} 
	if (cipher_cbc_op(&ini, &decrypt, cbc_buffer)) {
		cipher_free_session(crypto_dev, &ini); put_message_in_uart_queue("XERROR\n", strlen("XERROR\n")); return 0;
	}
	cipher_free_session(crypto_dev, &ini);

	return 1;
}


//process encryption
void * process_thread(void * x) 
{
	unsigned char* message;
	while(1)
	{
		//handle D, W, P in crpyto thread
		if(!k_msgq_get(&crypto_queue, &message, K_NO_WAIT)) {
			
			switch (message[0])
			{
			case 'D':
				printk("Process_thread: Decrypting\n");
				printk("Ciphertext: %02X\n", cbc_buffer);
				processing_busy = true; 
				if(cbc_mode())
				{
					format_plaintext_for_comparison(out_buffer);
				}
				processing_busy = false;
				break;
			case 'P':
				if(processing_busy == false){
					put_message_in_uart_queue("PROCESSING AVAILABLE\n", strlen("PROCESSING AVAILABLE\n"));
				}
				break; 
			case 'W':
				processing_busy = true; 
				sleep(10);
				processing_busy = false; 
				break; 
			
			
			default:
				break;
			}
		}
	}
	return x;
}


