#include "audio.h"
#include "arm_const_structs.h"
#define MEASURE_EXEC_TIME


volatile int16_t audio_chR = 0;
volatile int16_t audio_chL = 0;

float32_t x[2 * DMA_BUFFER_SIZE];


void DMA_HANDLER(void) {
    if (dstc_state(0)) {
		if(tx_proc_buffer == (PONG)) {
			dstc_src_memory(0,(uint32_t)&(dma_tx_buffer_pong));
			tx_proc_buffer = PING; 
		} else {
			dstc_src_memory(0,(uint32_t)&(dma_tx_buffer_ping));
			tx_proc_buffer = PONG; 
		}

		tx_buffer_empty = 1;
       	dstc_reset(0);
		}
		
	if (dstc_state(1)) {
		if (rx_proc_buffer == PONG) {
			dstc_dest_memory(1,(uint32_t)&(dma_rx_buffer_pong));
			rx_proc_buffer = PING;
		} else {
			dstc_dest_memory (1,(uint32_t)&(dma_rx_buffer_ping));
			rx_proc_buffer = PONG;
		}
		
		rx_buffer_full = 1;   		
		dstc_reset(1);
	}
}


void proces_buffer(void) {
	int i;
	uint32_t *txbuf, *rxbuf;
	float32_t out;
	
	if (tx_proc_buffer == PING) {
		txbuf = dma_tx_buffer_ping; 
	} else { 
		txbuf = dma_tx_buffer_pong;
	}
		
	if(rx_proc_buffer == PING) {
		rxbuf = dma_rx_buffer_ping;
	}	else {
		rxbuf = dma_rx_buffer_pong;
	}			 
		
	for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
		audio_IN = (*rxbuf++);
		audio_chL = (audio_IN & 0x0000FFFF);
		x[2*i] = (float32_t)(audio_chL);
		x[2*i+1] = 0.0f;
	}
	
	arm_cfft_f32(&arm_cfft_sR_f32_len128, x, 0, 1);
		
	for (i=0 ; i <(DMA_BUFFER_SIZE); i++) {
		arm_sqrt_f32(x[2*i] * x[2*i] + x[2*i+1] * x[2*i+1], &out);
		out = ((out > 32767) ? (32767) : (out));
		*txbuf++ = (uint32_t)((((short)(out)<<16 & 0xFFFF0000)) + ((short)(out) & 0x0000FFFF));	
	}
	
	tx_buffer_empty = 0;
	rx_buffer_full = 0;
}


int main (void) {
#ifdef MEASURE_EXEC_TIME
	gpio_set_mode(P15, Output);
#endif
		
	audio_init(hz8000, mic_in, dma, DMA_HANDLER);

	while (1) {
		while (!(rx_buffer_full && tx_buffer_empty));
			
#ifdef MEASURE_EXEC_TIME
		gpio_set(P15, HIGH);
#endif	
		proces_buffer();
#ifdef MEASURE_EXEC_TIME
		gpio_set(P15, LOW);
#endif
	}
}
