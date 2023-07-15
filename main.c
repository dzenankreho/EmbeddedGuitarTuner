#include "audio.h"
#include "arm_const_structs.h"
#include "soundCommands.h"

#define MEASURE_EXEC_TIME_
#define BUFFER_SIZE 16*DMA_BUFFER_SIZE
#define MEAN_THRESHOLD 1e4f
#define VAR_THRESHOLD 1e10f
#define FREQ_THRESHOLD 30.0f

typedef enum { PLAYING_COMMAND, LISTENING } OperationMode;
OperationMode operationMode = PLAYING_COMMAND;

float32_t lowerBounds[] = {77.8f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f};
float32_t upperBounds[] = {87.3f, 116.5f, 155.6f, 207.7f, 261.6f, 349.2f};

volatile int16_t audio_chR = 0;
volatile int16_t audio_chL = 0;

float32_t y[BUFFER_SIZE] = { 0 };
float32_t x[2 * DMA_BUFFER_SIZE] = { 0 };
float32_t buffer[2 * BUFFER_SIZE] = { 0 };
float32_t buffer2[BUFFER_SIZE] = { 0 };
int cnt = 0;
float32_t max;
uint32_t maxIndex;
float32_t freq = 0;
float32_t mean = 0;
float32_t var = 0;
int32_t soundCommandCnt = 0;
int string = -1;

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
		
	if (operationMode == LISTENING) {
		for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
			audio_IN = (*rxbuf++);
			audio_chL = (audio_IN & 0x0000FFFF);
			x[2*i] = (float32_t)(audio_chL);
			y[cnt] = x[2*i];
			buffer[2*cnt] = x[2*i];
			x[2*i+1] = 0.0f;
			buffer[2*cnt+1] = 0.0f;
			cnt++;
		}
	}
	
	switch (operationMode) {
		case PLAYING_COMMAND:
			for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
				*txbuf++ = ((popustiteZicu[soundCommandCnt] << 16 & 0xFFFF0000)) + (zategniteZicu[soundCommandCnt] & 0x0000FFFF);
				if (++soundCommandCnt == DURATION) {
					operationMode = LISTENING;
					break;
				}
			}
			break;
			
		case LISTENING:
			for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
				*txbuf++ = 0;	
			}
			break;
		
		default:
			break;
	}
	
	tx_buffer_empty = 0;
	rx_buffer_full = 0;
}


int main (void) {
	int j = 0;
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
		if (cnt == BUFFER_SIZE) {
			arm_cfft_f32(&arm_cfft_sR_f32_len2048, buffer, 0, 1);
			arm_cmplx_mag_f32(buffer, buffer2, BUFFER_SIZE);
			cnt = 0;
			arm_mean_f32(buffer2, BUFFER_SIZE / 2, &mean);
			arm_var_f32(buffer2, BUFFER_SIZE / 2, &var);
			arm_max_f32(buffer2, BUFFER_SIZE / 2, &max, &maxIndex);
			freq = (maxIndex / 1024.0f) * 4000.0f;
			if (mean > MEAN_THRESHOLD && var > VAR_THRESHOLD && freq > 30.0f) {
				for (j = 0; j < 6; j++) {
					if ((lowerBounds[j] <= freq && upperBounds[j] >= freq) ||
						(lowerBounds[j] <= freq / 2 && upperBounds[j] >= freq / 2) ||
						(lowerBounds[j] <= freq / 3 && upperBounds[j] >= freq / 3) ) {
							string = j;
							break;
						}
				}
				soundCommandCnt = 0;
				operationMode = PLAYING_COMMAND;
			} else {
				freq = -1;
				string = -1;
			}
		}
#ifdef MEASURE_EXEC_TIME
		gpio_set(P15, LOW);
#endif
	}
}
