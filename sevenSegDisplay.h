#pragma once
#include "gpio.h"


extern Pin a;
extern Pin b;
extern Pin c;
extern Pin d;
extern Pin e;
extern Pin f;
extern Pin g;
extern Pin enable1;
extern Pin enable2;
extern Pin enable3;


void init7segDisplays();
void clearDisplay();
void displayNo0();
void displayNo1();
void displayNo2();
void displayNo3();
void displayNo4();
void displayNo5();
void displayNo6();
void displayNo7();
void displayNo8();
void displayNo9();
void displayNo(int16_t number);
void enableDisplay(int16_t displayNumber);
void disableDisplay(int16_t displayNumber);