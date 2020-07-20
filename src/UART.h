/*
---------------------------------
UART module for
ARDUINO NANO
---------------------------------
*/

void UART0_Init(unsigned int baud);
void UART0_sendChar(char data);
char UART0_receiveChar();
void UART0_puts(char *string);
void UART0_gets(char *c_buffer);
void UART0_enableReceive_Itr();
void UART0_disableReceive_Itr();
void bob(char str[], int value);
