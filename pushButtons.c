#include "pushButtons.h"



Pin switchMode = PF0;
Pin switchString = P41;
Pin switchTune = P40;



void initPushButtons(void) {
	gpio_set_mode(switchMode, PullUp_Input);
	gpio_set_mode(switchString, PullUp_Input);
	gpio_set_mode(switchTune, PullUp_Input);
}
