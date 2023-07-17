#include "sevenSegDisplay.h"
#include "gpio.h"

// 1 2 4 6 7 9 10 3 3'
Pin a = PF5;
Pin b = P43;
Pin c = PF7;
Pin d = P1C;
Pin e = P1B;
Pin f = P44;
Pin g = P45;
Pin enable1 = PF3;
Pin enable2 = P7D;
Pin enable3 = PF1;


void init7segDisplays(void) {
	gpio_set_mode(a, Output);
	gpio_set(a, LOW);
	gpio_set_mode(b, Output);
	gpio_set(b, LOW);
	gpio_set_mode(c, Output);
	gpio_set(c, LOW);
	gpio_set_mode(d, Output);
	gpio_set(d, LOW);
	gpio_set_mode(e, Output);
	gpio_set(e, LOW);
	gpio_set_mode(f, Output);
	gpio_set(f, LOW);
	gpio_set_mode(g, Output);
	gpio_set(g, LOW);
	gpio_set_mode(enable1, Output);
	gpio_set(enable1, HIGH);
	gpio_set_mode(enable2, Output);
	gpio_set(enable2, HIGH);
	gpio_set_mode(enable3, Output);
	gpio_set(enable3, HIGH);	
}


void displayNo0(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, LOW);
}


void displayNo1(void) {
	gpio_set(a, LOW);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo2(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, LOW);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, LOW);
	gpio_set(g, HIGH);
}


void displayNo3(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, HIGH);
}


void displayNo4(void) {
	gpio_set(a, LOW);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo5(void) {
	gpio_set(a, HIGH);
	gpio_set(b, LOW);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, LOW);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo6(void) {
	gpio_set(a, HIGH);
	gpio_set(b, LOW);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo7(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo8(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo9(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, LOW);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo(int number) {
	if (number > 9 || number < -9) {
		return;
	}
	
	
	if (number < 0) {
		number = -number;
	}
	
	
	switch (number) {
		case 0:
			displayNo0();
			break;
		case 1:
			displayNo1();
			break;
		case 2:
			displayNo2();
			break;
		case 3:
			displayNo3();
			break;
		case 4:
			displayNo4();
			break;
		case 5:
			displayNo5();
			break;
		case 6:
			displayNo6();
			break;
		case 7:
			displayNo7();
			break;
		case 8:
			displayNo8();
			break;
		case 9:
			displayNo9();
			break;
	}
}


void clearDisplay(void) {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void enableDisplay(int displayNumber) {
	switch (displayNumber) {
		case 0:
			gpio_set(enable1, LOW);
			break;
		case 1:
			gpio_set(enable2, LOW);
			break;
		case 2:
			gpio_set(enable3, LOW);
			break;
	}
}


void disableDisplay(int displayNumber) {
	switch (displayNumber) {
		case 0:
			gpio_set(enable1, HIGH);
			break;
		case 1:
			gpio_set(enable2, HIGH);
			break;
		case 2:
			gpio_set(enable3, HIGH);
			break;
	}
}


void displayMinus(void) {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, HIGH);
}

void displayPlus(void) {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, LOW);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}
