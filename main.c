#include "audio.h"
#include "arm_const_structs.h"
#include "soundCommands.h"

#define BUFFER_SIZE 16*DMA_BUFFER_SIZE
#define MEAN_THRESHOLD 1e4f
#define VAR_THRESHOLD 1e10f
#define FREQ_THRESHOLD 30.0f

typedef enum { PLAYING_COMMAND, LISTENING } OperationMode;
OperationMode operationMode = LISTENING;

float32_t lowerBounds[] = {77.8f, 103.8f, 138.6f, 185.0f, 233.1f, 311.1f};
float32_t upperBounds[] = {87.3f, 116.5f, 155.6f, 207.7f, 261.6f, 349.2f};
float32_t tunedStrings[] = {82.4f, 110.0f, 146.8f, 196.0f, 246.9f, 329.6f};

volatile int16_t audio_chR = 0;
volatile int16_t audio_chL = 0;

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
float32_t logHelper1 = 0, logHelper2 = 0;
int32_t n = 0, nScaled = 0;
int16_t* soundCommand = 0;
int32_t soundCommandDuration  = 0;

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
			buffer[2*cnt] = (float32_t)(audio_chL);
			buffer[2*cnt+1] = 0.0f;
			cnt++;
		}
	}
	
	switch (operationMode) {
		case PLAYING_COMMAND:
			for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
				*txbuf++ = ((soundCommand[soundCommandCnt] << 16 & 0xFFFF0000)) + (soundCommand[soundCommandCnt] & 0x0000FFFF);
				if (++soundCommandCnt == soundCommandDuration) {
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
	/*
	// 1 2 4 6 7 9 10 3 3'
	Pin pins[] = {P1B, P1C, PF7, P43, PF5, P44, P45, PF3, P7D};
	int j = 0;
	for (j = 0; j < 9; j++) {
		gpio_set_mode(pins[j], Output);
		gpio_set(pins[j], ((j<7)?(HIGH):(LOW)));
	}
	
	gpio_set(pins[0], HIGH);
	gpio_set(pins[1], LOW);
	gpio_set(pins[2], LOW);
	gpio_set(pins[3], LOW);
	gpio_set(pins[4], LOW);
	gpio_set(pins[5], HIGH);
	gpio_set(pins[6], LOW);
	
	
	while(1);
	*/
	
	int j = 0;	
		
	audio_init(hz8000, mic_in, dma, DMA_HANDLER);

	while (1) {
		while (!(rx_buffer_full && tx_buffer_empty));

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
					if (lowerBounds[j] <= freq && upperBounds[j] >= freq) {
						string = 6 - j;
						break;
					}
					if (lowerBounds[j] <= freq / 2 && upperBounds[j] >= freq / 2) {
						freq /= 2;
						string = 6 - j;
						break;
					}
				}
				if (string != -1) {
					logHelper1 = freq / tunedStrings[j];
					arm_vlog_f32(&logHelper1, &logHelper2, 1);
					n = 1731.23404907f * logHelper2;
					nScaled = n / 10;
					soundCommandCnt = 0;
					if (nScaled < 0) {
						soundCommand = zategniteZicu;
						soundCommandDuration = ZATEGNITE_ZICU_DURATION;
					} else if (nScaled > 0) {
						soundCommand = popustiteZicu;
						soundCommandDuration = POPUSTITE_ZICU_DURATION;
					} else {
						soundCommand = zicaNastimana;
						soundCommandDuration = ZICA_NASTIMANA_DURATION;
					}	
					operationMode = PLAYING_COMMAND;
				}
			} else {
				freq = -1;
				string = -1;
			}
		}
	}
}
