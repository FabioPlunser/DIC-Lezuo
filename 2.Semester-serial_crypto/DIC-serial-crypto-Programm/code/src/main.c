/*
	Author: Fabio Plunser

*/
#include "main.h"


const struct device * uart_dev;

//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#_CPPv411uart_config

void main(void)
{
	//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#api-reference
	//int uart_configure(structdevice *dev, conststructuart_config *cfg)
	uart_dev = device_get_binding("UART_0");
	

	pthread_t thread_id[Number_of_threads];
	init_threads(thread_id);
	
}

void init_threads(pthread_t* thread_id)
{
	int i, thread_ok;
	void*(*threads[])(void*) = {uart_in_thread, uart_out_thread, process_thread};
	for(int i=0; i<Number_of_threads; i++)
	{
		thread_ok = pthread_create(&thread_id[i], NULL, threads[i], NULL);
		if(thread_ok != 0)
		{
			printk("Thred creation Error\n");
		}
	}
	
}


void* uart_in_thread(void * x)
{
	while(1)
	{
		printk("uart_in_thread\n\n");
		sleep(1);
	}
	return x;
}


void* uart_out_thread(void * x)
{
	char* string="Test";
	while(1)
	{
		
		for(int i=0; i<strlen(string); i++)
		{
			uart_poll_out(uart_dev, string[i]);
		}
		
		printk("uart_out_thread\n\n");
		sleep(1);
	}
	return x;
}

void * process_thread(void * x) 
{
	while(1)
	{
		printk("process_thread\n\n");
		sleep(1);
	}
	return x;
}

