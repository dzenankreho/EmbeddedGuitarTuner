#include "audio.h"
#include "arm_const_structs.h"
#include "soundCommands.h"
#include "sevenSegDisplay.h"
#include "pushButtons.h"
#include "tuning.h"



#define BUFFER_SIZE 16 * DMA_BUFFER_SIZE
#define MEAN_THRESHOLD 1e4f
#define VAR_THRESHOLD 1e10f
#define FREQ_THRESHOLD 50.0f
#define CONST_1200_DIV_LN2 1731.23404907f


enum { PLAYING_COMMAND, LISTENING } operationMode = LISTENING;

enum { AUTO, MANUAL } tunerMode = AUTO;
int32_t manualStringCnt = 0;

Tuning* tunings[] = {&standardTuning, &dropDTuning, &fullStepDownTuning};
Tuning* tuning = &standardTuning;
int32_t tuningSwitchCnt = 0;

volatile int16_t audio_chL = 0;

float32_t buffer[2 * BUFFER_SIZE] = { 0 };
float32_t buffer2[BUFFER_SIZE] = { 0 };
int32_t cnt = 0;

int32_t nScaled = 0;
int16_t string = -1;

int16_t* soundCommand = 0;
int32_t soundCommandDuration  = 0;
int32_t soundCommandCnt = 0;

int32_t sevenSegDisplaysCnt = 0;

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
	
	switch (sevenSegDisplaysCnt) {
		case 0:
			disableDisplay(2);
			enableDisplay(0);
			displayNo(((string != -1) ? (nScaled % 10) : ((int16_t)tunerMode)));
			break;
		case 1:
			disableDisplay(0);
			enableDisplay(1);
			displayNo(((string != -1) ? (nScaled / 10) : (tuningSwitchCnt)));
			break;
		case 2:
			disableDisplay(1);
			enableDisplay(2);
			if (string != -1) {
				if (nScaled < 0) {
					displayMinus();
				} else {
					displayPlus();
				}
			} else if (tunerMode == MANUAL) {
				displayNo(6 - manualStringCnt);
			} else {
				clearDisplay();
			}
			break;
		default:
			break;
	}
	
	sevenSegDisplaysCnt = (sevenSegDisplaysCnt + 1) % 3;
}



void procesDMAbuffer(void) {
	int32_t i;
	uint32_t *txbuf, *rxbuf;
	
	
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
		// Transfer data into buffer
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
			// Send voice message 
			for (i = 0; i < (DMA_BUFFER_SIZE); i++) {
				*txbuf++ = ((soundCommand[soundCommandCnt] << 16 & 0xFFFF0000)) + (soundCommand[soundCommandCnt] & 0x0000FFFF);
				if (++soundCommandCnt == soundCommandDuration) {
					operationMode = LISTENING;
					break;
				}
			}
			break;
			
		case LISTENING:
			// Send "silence"
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



void procesDataBuffer() {
	int8_t j = 0;	
	float32_t max = 0.;
	uint32_t maxIndex = 0;
	float32_t freq = 0;
	float32_t mean = 0;
	float32_t var = 0;	
	float32_t logHelper1 = 0;
	float32_t logHelper2 = 0;
	int32_t n = 0;
	
	
	// Find |H(jw)|
	arm_cfft_f32(&arm_cfft_sR_f32_len2048, buffer, 0, 1);
	arm_cmplx_mag_f32(buffer, buffer2, BUFFER_SIZE);
	
	
	// Find mean, var and frequency with max |H(jw)|
	arm_mean_f32(buffer2, BUFFER_SIZE / 2, &mean);
	arm_var_f32(buffer2, BUFFER_SIZE / 2, &var);
	arm_max_f32(buffer2, BUFFER_SIZE / 2, &max, &maxIndex);
	freq = (maxIndex / 1024.0f) * 4000.0f;
	
	
	
	if (mean > MEAN_THRESHOLD && var > VAR_THRESHOLD && freq > FREQ_THRESHOLD) {
		switch (tunerMode) {
			case AUTO:
				// Find the played string and the frequency of the first harmonic
				for (j = 0; j < 6; j++) {
					if (tuning->lowerBounds[j] <= freq && tuning->upperBounds[j] >= freq) {
						string = 6 - j;
						break;
					}
					if (tuning->lowerBounds[j] <= freq / 2 && tuning->upperBounds[j] >= freq / 2) {
						freq /= 2;
						string = 6 - j;
						break;
					}
				}
				break;
				
			case MANUAL:
				// Find the frequency of the first harmonic
				if (tuning->lowerBounds[manualStringCnt] <= freq && tuning->upperBounds[manualStringCnt] >= freq) {
					string = 6 - manualStringCnt;
					break;
				}
				if (tuning->lowerBounds[manualStringCnt] <= freq / 2 && tuning->upperBounds[manualStringCnt] >= freq / 2) {
					freq /= 2;
					string = 6 - manualStringCnt;
					break;
				}
				break;
				
			default:
				break;
		}
	
		
		if (string != -1) {
			// Find frequency deviation in cents
			logHelper1 = freq / tuning->tunedStrings[((tunerMode == AUTO) ? (j) : (manualStringCnt))];
			arm_vlog_f32(&logHelper1, &logHelper2, 1);
			n = CONST_1200_DIV_LN2 * logHelper2;
			nScaled = n / 10;
			
			
			// Choose appropriate message
			soundCommandCnt = 0;
			if (nScaled < -tuning->tolerance) {
				soundCommand = zategniteZicu;
				soundCommandDuration = ZATEGNITE_ZICU_DURATION;
			} else if (nScaled > tuning->tolerance) {
				soundCommand = popustiteZicu;
				soundCommandDuration = POPUSTITE_ZICU_DURATION;
			} else {
				soundCommand = zicaNastimana;
				soundCommandDuration = ZICA_NASTIMANA_DURATION;
			}	
			operationMode = PLAYING_COMMAND;
		}
	} else {
		string = -1;
	}
	
	
	cnt = 0;
}



int main (void) {
	initPushButtons();
	init7segDisplays();
	
	audio_init(hz8000, mic_in, dma, DMA_HANDLER);

	while (1) {
		if (rx_buffer_full && tx_buffer_empty) {
			procesDMAbuffer();
		
			if (cnt == BUFFER_SIZE) {
				procesDataBuffer();
			}
		}
		
		
		if (!gpio_get(switchMode)) {
			delay_ms(300);
			tunerMode = ((tunerMode == AUTO) ? (MANUAL) : (AUTO));
			manualStringCnt = 0;
		}
		
		
		if (!gpio_get(switchString)) {
			delay_ms(300);
			manualStringCnt = (manualStringCnt + 1) % 6;
		}	
		
		
		if (!gpio_get(switchTune)) {
			delay_ms(300);
			tuningSwitchCnt = (tuningSwitchCnt + 1) % NUMBER_OF_TUNINGS;
			tuning = tunings[tuningSwitchCnt];
		}
	}
}
