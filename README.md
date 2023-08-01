#  Embedded Guitar Tuner
A Guitar Tuner for acoustic guitars implemented on an embedded system.  

This application/project was developed as a part of the graduate elective course "Software Development for Embedded Systems", which is part of the Automatic control and electronics Master's degree program offered by the Faculty of Electrical Engineering, University of Sarajevo.

The goal of the project was to create a guitar tuner for acoustic guitars that runs on a FM4-176L-S6E2CC-ETH development board.

Sound acquisition is done through a microphone connected to the microphone input of the mentioned development board. When a guitar string is played, a sound command is issued via the headphone output of the board. The sound command tells whether the string is tuned or whether it needs loosening or tightening. The deviation of the frequency of the played string from the tuned string is calculated and shown in the cent measuring unit using three seven-segment displays connected to the board via digital outputs. The tuner has an automatic mode (the played string is detected automatically) and a manual mode (only the selected string is detected). Apart from the standard tuning, Drop D and Full step down are also supported. Selection of the mode and tuning is done using three pushbuttons connected to the board via digital inputs. Then a string is not played, the seven-segment displays show the current setting, that is, the mode, tuning and the selected string if the manual mode is used.

The application itself is written in C and uses the [CMSIS DSP](https://github.com/ARM-software/CMSIS-DSP) library. 

Project done by:
- [Dženan Kreho](https://github.com/dkreho1)
