/*
    
    Kasper Nyhus Kaae
        s195087

---------------------------------
ADC module for
ARDUINO MEGA2560
---------------------------------

*/

#include "adc.h"


void adc1_125khz_timer1_OV_Init() {
	ADCSRA |= (1<<ADEN); //ADC Enable
	ADCSRA |= (1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); //prescaling 16Mhz/128 = 125kHz DS: p.293
	ADMUX |= (1<<REFS0); // AVCC reference
	
	ADCSRA |= (1<<ADIE); // ADC Interrupt Enable

	ADCSRA |= (1<<ADATE); //ADC Auto Trigger Enable
	ADCSRB |= (1<<ADTS1)|(1<<ADTS2); //ADC Auto Trigger Source Timer1 Overflow
	
	ADMUX |= (1<<MUX0); // channel activated: ADC1 
	ADCSRA |= (1<<ADSC); //ADC Start Converting
}



unsigned int get_sample(char channel) {
	
	DIDR0 = (1<<channel);
	DIDR0 =~ (1<<channel); //now only active pin in "Digital Input Disable Register" is the current channel
	DIDR0 = 0xFE;
	DIDR1 = 0xFF; //every other channel is off

	ADCSRA |= (1<<ADSC); //Start Converting
	while ( !(ADCSRA & (1<<ADIF))) { //wait until sample complete (ADIF = ADC Interrupt Flag)

	}
	
	return (ADCL+ (ADCH<<8));
}


