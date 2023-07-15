#include "sevenSegDisplay.h"


Pin a = P1B;
Pin b = P1C;
Pin c = PF7;
Pin d = P43;
Pin e = PF5;
Pin f = P44;
Pin g = P45;
Pin enable1 = PF3;
Pin enable2 = P7D;
Pin enable3 = PF1;


void init7segDisplays() {
	Pin pins[] = {a, b, c, d, e, f, g, enable1, enable2, enable3};
	int i = 0;
	for (i = 0; i < 10; i++) {
		gpio_set_mode(pins[i], Output);
		gpio_set(pins[i], ((i < 7) ? (HIGH) : (LOW)));
	}
}


void displayNo0() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, HIGH);
}


void displayNo1() {
	gpio_set(a, HIGH);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo2() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, HIGH);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, HIGH);
	gpio_set(g, LOW);
}


void displayNo3() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, LOW);
}


void displayNo4() {
	gpio_set(a, HIGH);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo5() {
	gpio_set(a, LOW);
	gpio_set(b, HIGH);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, HIGH);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo6() {
	gpio_set(a, LOW);
	gpio_set(b, HIGH);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo7() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void displayNo8() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, LOW);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo9() {
	gpio_set(a, LOW);
	gpio_set(b, LOW);
	gpio_set(c, LOW);
	gpio_set(d, LOW);
	gpio_set(e, HIGH);
	gpio_set(f, LOW);
	gpio_set(g, LOW);
}


void displayNo(int16_t number) {
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


void clearDisplay() {
	gpio_set(a, HIGH);
	gpio_set(b, HIGH);
	gpio_set(c, HIGH);
	gpio_set(d, HIGH);
	gpio_set(e, HIGH);
	gpio_set(f, HIGH);
	gpio_set(g, HIGH);
}


void enableDisplay(int16_t displayNumber) {
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


void disableDisplay(int16_t displayNumber) {
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
