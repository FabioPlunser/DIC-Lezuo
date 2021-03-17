/*
	Author: Fabio Plunser

*/
#include "main.h"


struct uart_message{
	unsigned char* message;
	uint32_t len; 
};
//https://docs.zephyrproject.org/2.3.0/reference/kernel/data_passing/message_queues.html?highlight=queue
K_MSGQ_DEFINE(uart_queue, sizeof(struct uart_message* ), 20, 1);
K_MSGQ_DEFINE(crypto_queue, sizeof(char* ), 20, 1);


const struct device * uart_dev;
const struct device * crypto_dev;
enum state status = INIT;

bool processing_busy = false; 


//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#_CPPv411uart_config


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
	uint8_t uart_in = '\0';
	printk("In State Machine\n");
	while(1)
	{
		switch(status){
			case INIT: 
				// printk("Init State\n");
				if(!uart_poll_in(uart_dev, &uart_in)){
					printk("uart_in: %c\n", uart_in);
					switch(uart_in){
						case '.':
							put_message_in_uart_queue(".\r\n");
							if(processing_busy == false)
							{
								put_message_in_uart_queue("BUSY\r\n");
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
			default:
				break;
		}
	}
}

void* uart_in_thread(void * x){
	
	state_machine();
	// uint8_t uart_in = '\0';
	// while(1)
	// {
	// 	uart_poll_in(uart_dev,&uart_in);
	// 	printk("uart_in: %c\n", uart_in);
	// }
	
	return x;

	// static struct uart_message message;
	// message.message = "Hallo";
	// message.len = strlen("Hallo");

	// char* string; 
	// uint32_t len; 
	// struct uart_message * message_pointer = &message;
	// string = message_pointer->message;
	// len = message_pointer->len; 
	// k_msgq_put(&uart_queue, &message_pointer, K_FOREVER);
	
	
	// while(1)
	// {	
	// }
	// return x;
}

int put_message_in_uart_queue(unsigned char* str)
{	
	static struct uart_message message;
	message.message = str;
	message.len = strlen(str);

	
	char* string; 
	uint32_t len; 
	struct uart_message * message_pointer = &message;
	string = message_pointer->message;
	len = message_pointer->len; 

	printk("Putting %s into uart_queue\n", str);
	if(k_msgq_put(&uart_queue, &message_pointer, K_FOREVER)!=0){
		printk("Couldn't put message in queue!!\n");
	}
	return 0;
}
int put_message_in_crypto_queue(unsigned char* str)
{
	static struct uart_message message;
	message.message = str;
	message.len = strlen(str);


	char* string; 
	uint32_t len; 
	struct uart_message * message_pointer = &message;
	string = message_pointer->message;
	len = message_pointer->len; 


	k_msgq_put(&uart_queue, &message_pointer, K_FOREVER);
	return 0;
}
void* uart_out_thread(void * x)
{
	int i=0;
	struct uart_message * message;
	char* tx; 
	uint32_t len; 
	// char* string="Test\n";

	
	while(1)
	{
		// printk("uart_out_thread_alive\n");
		if(!k_msgq_get(&uart_queue, &message, K_NO_WAIT)) {
			tx = message->message;
			len = message->len;
			printk("Message in queue: %s, leng: %i\n", tx, len);

			while(i < len)
			{
				uart_poll_out(uart_dev, tx[i++]);
			}
			i = 0;
		}
	}
	return x;
}

void * process_thread(void * x) 
{
	while(1)
	{
		sleep(0.5);
	}
	return x;
}


