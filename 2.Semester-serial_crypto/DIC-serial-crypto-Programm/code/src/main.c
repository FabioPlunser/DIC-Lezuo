/*
	Author: Fabio Plunser

*/
#include "main.h"


//https://docs.zephyrproject.org/2.3.0/reference/kernel/data_passing/message_queues.html?highlight=queue
K_MSGQ_DEFINE(uart_queue, sizeof(char* ), q_max_msgs, q_align);
K_MSGQ_DEFINE(crypto_queue, sizeof(char* ), q_max_msgs, q_align);


const struct device * uart_dev;
const struct device * crypto_dev;
enum state status = INIT;

bool processing_busy = false;  
//https://docs.zephyrproject.org/2.3.0/reference/peripherals/uart.html#_CPPv411uart_config

//Random ciphertext key and plaintext from cbc sample
static const uint8_t cbc_ciphertext[80] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46,
	0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d, 0x50, 0x86, 0xcb, 0x9b,
	0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
	0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e,
	0x22, 0x22, 0x95, 0x16, 0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09,
	0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7
};

static uint8_t key[16] = {
	0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
	0x09, 0xcf, 0x4f, 0x3c
};

static uint8_t plaintext[64] = {
	0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11,
	0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
	0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, 0x30, 0xc8, 0x1c, 0x46,
	0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
	0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b,
	0xe6, 0x6c, 0x37, 0x10
};
uint32_t cap_flags;

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
	unsigned char uart_in;
	printk("In State Machine\n");
	while(1)
	{
		switch(status){
			case INIT: 
				// printk("Init State\n");
				if(!uart_poll_in(uart_dev, &uart_in)){
					printk("uart_in: %c\n", uart_in);
					switch(uart_in){
						case 'D':
						case 'd':
							if(processing_busy == false){
								status = DECRYPT;
							}
							
							break;
						case 'K':
						case 'k':
							if(processing_busy == false)
							{
								status = KEY;
							}
							break;
						case 'I':
						case 'i':
							if(processing_busy == false){
								status = IV;
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
			default:
				break;
			case DECRYPT:
				processing_busy = true;
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

int validate_hw_compatibility(const struct device *dev)
{
	uint32_t flags = 0U;

	flags = cipher_query_hwcaps(dev);
	if ((flags & CAP_RAW_KEY) == 0U) {
		// LOG_INF("Please provision the key separately "
		// 	"as the module doesnt support a raw key");
		return -1;
	}

	if ((flags & CAP_SYNC_OPS) == 0U) {
		// LOG_ERR("The app assumes sync semantics. "
		//   "Please rewrite the app accordingly before proceeding");
		return -1;
	}

	if ((flags & CAP_SEPARATE_IO_BUFS) == 0U) {
		// LOG_ERR("The app assumes distinct IO buffers. "
		// "Please rewrite the app accordingly before proceeding");
		return -1;
	}

	cap_flags = CAP_RAW_KEY | CAP_SYNC_OPS | CAP_SEPARATE_IO_BUFS;

	return 0;

}


void cbc_mode(const struct device *dev)
{
	uint8_t encrypted[80] = {0};
	uint8_t decrypted[64] = {0};
	struct cipher_ctx ini = {
		.keylen = sizeof(key),
		.key.bit_stream = key,
		.flags = cap_flags,
	};
	struct cipher_pkt encrypt = {
		.in_buf = plaintext,
		.in_len = sizeof(plaintext),
		.out_buf_max = sizeof(encrypted),
		.out_buf = encrypted,
	};
	struct cipher_pkt decrypt = {
		.in_buf = encrypt.out_buf,
		.in_len = sizeof(encrypted),
		.out_buf = decrypted,
		.out_buf_max = sizeof(decrypted),
	};

	static uint8_t iv[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	};

	if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES,
				 CRYPTO_CIPHER_MODE_CBC,
				 CRYPTO_CIPHER_OP_ENCRYPT)) {
		return;
	}

	if (cipher_cbc_op(&ini, &encrypt, iv)) {
		printk("CBC mode ENCRYPT - Failed\n");
		goto out;
	}

	printk("Output length (encryption): %d\n", encrypt.out_len);

	if (memcmp(encrypt.out_buf, cbc_ciphertext, sizeof(cbc_ciphertext))) {
		printk("CBC mode ENCRYPT - Mismatch between expected and "
			    "returned cipher text\n");
		put_message_in_crypto_queue((char*)cbc_ciphertext);
		// print_buffer_comparison(cbc_ciphertext, encrypt.out_buf,
		// 			sizeof(cbc_ciphertext));
		goto out;
	}

	printk("CBC mode ENCRYPT - Match\n");
	cipher_free_session(dev, &ini);

	if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES,
				 CRYPTO_CIPHER_MODE_CBC,
				 CRYPTO_CIPHER_OP_DECRYPT)) {
		return;
	}

	/* TinyCrypt keeps IV at the start of encrypted buffer */
	if (cipher_cbc_op(&ini, &decrypt, encrypted)) {
		printk("CBC mode DECRYPT - Failed\n");
		goto out;
	}

	printk("Output length (decryption): %d,", decrypt.out_len);

	if (memcmp(decrypt.out_buf, plaintext, sizeof(plaintext))) {
		printk("CBC mode DECRYPT - Mismatch between plaintext and "
			    "decrypted cipher text\n");
		put_message_in_crypto_queue((char*)plaintext);
		// print_buffer_comparison(plaintext, decrypt.out_buf,
		// 			sizeof(plaintext));
		goto out;
	}

out:
	cipher_free_session(dev, &ini);
}

// void AES_CBC()
// {
// 	//https://docs.zephyrproject.org/latest/reference/crypto/index.html?highlight=cipher_cbc_op#overview
// 	struct cipher_ctx ctx{
// 		.key.bit_stream = key,
// 		.keylen = AES_KEY_LEN,
// 		.flags = 
// 	};
// 	struct cipher_pkt pkt{

// 	}
// 	if(!cipher_begin_session(crypto_dev, &ctx, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CBC, CRYPTO_CIPHER_OP_DECRYPT)){
// 		printk("Couldn't create cipher session\n");
// 	}
// 	if(!cipher_cbc_op(&ctx, &pkt, iv)){
// 		printk("Couldn't do cbc operation\n");
// 	}
// }
//process encryption
void * process_thread(void * x) 
{
	int i=0;
	uint32_t len; 

	unsigned char* message;
	while(1)
	{
		// printk("uart_out_thread_alive\n");
		if(!k_msgq_get(&crypto_queue, &message, K_NO_WAIT)) {
			// tx = message->message;
			// len = message->len;
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


