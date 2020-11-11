/*
---------------------------------
UART module for
ARDUINO NANO
---------------------------------
*/

#include <avr/io.h>
#include "UART.h"


void UART0_Init(unsigned int baud) {
  UCSR0A = (1<<U2X0); //Enable dublex. OBS: ONLY use with async operation
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0); //Enable receive and transmit
  //UCSR0C &=~ (1<<UMSEL00) | (1<<UMSEL01); //Async UART
  UCSR0C &=~ (1<<UPM00) | (1<<UPM01) | (1<<USBS0); //No parity. 1 stop bit
  UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01); //8-bit data frame

  //Baud rate setting
  UBRR0H = (unsigned char)(baud>>8);
  UBRR0L = (unsigned char)(baud);
}


void UART0_enableReceive_Itr() {
  UCSR0B |= (1<<RXCIE0);
}

void UART0_disableReceive_Itr() {
  UCSR0B &=~ (1<<RXCIE0);
}


void UART0_sendChar(char data) {
  while ( !( UCSR0A & (1<<UDRE0)) ); //Wait for empty transmit buffer
  UDR0 = data; //Load data into UART Data Register
}
 
char UART0_receiveChar() {
  while ( !(UCSR0A & (1<<RXC0)) ); //Wait until char is received
  return UDR0; //return data from UART Data Register
}

void UART0_puts(char *StringPtr) {
  while(*StringPtr != '\0') { //while char in string is not Carriage-Return (!= '\r')
    UART0_sendChar(*StringPtr);
    StringPtr++;
  }
}

void UART0_gets(char *c_buffer) {
  char c;
  while((c=UART0_receiveChar()) != 0x0D) { // while not "Enter"
    *c_buffer = c;
    UART0_sendChar(c);
    c_buffer++;
  }
  *c_buffer = '\n';
  c_buffer++;
  *c_buffer = '\r';
  c_buffer++;
  *c_buffer = 0; // terminate string
}


void bob(char str[], int value) {
  char c_buffer[50];
  
  UART0_puts(str);
  sprintf(c_buffer, "%d\r\n", value);
  UART0_puts(c_buffer);
}


// Up to 16-bits
void UART0_printBits(char str[], uint32_t bitValue, uint16_t bitdepth) {
  UART0_puts(str);

  if (bitdepth > 16) {
    UART0_puts("Error");
    return;
  }
  
  for (int i=0; i<bitdepth; i++) {
    int bit_mask = (1<<(bitdepth-1));
    int nextBit = ((bitValue<<i) & bit_mask);
    if (nextBit == 0) {
      UART0_sendChar('0');
    }
    else {
      UART0_sendChar('1');
    }
  }
  UART0_puts("\r\n");
}

void UART0_printBits_nonewline(char str[], uint32_t bitValue, uint16_t bitdepth) {
  UART0_puts(str);

  if (bitdepth > 16) {
    UART0_puts("Error");
    return;
  }
  
  for (int i=0; i<bitdepth; i++) {
    int bit_mask = (1<<(bitdepth-1));
    int nextBit = ((bitValue<<i) & bit_mask);
    if (nextBit == 0) {
      UART0_sendChar('0');
    }
    else {
      UART0_sendChar('1');
    }
  }
}