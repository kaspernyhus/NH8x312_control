/*
    
    Kasper Nyhus Kaae
        s195087

---------------------------------
ADC module for
ARDUINO MEGA2560
---------------------------------

*/

#include <avr/io.h>
#include <avr/interrupt.h>

void adc1_125khz_timer1_OV_Init();
unsigned int get_sample(char channel);