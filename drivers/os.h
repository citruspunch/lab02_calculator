#ifndef OS_H
#define OS_H

#define UART0_BASE 0x101f1000

#define UART_DR      0x00  // Data Register
#define UART_FR      0x18  // Flag Register
#define UART_FR_TXFF 0x20  // Transmit FIFO Full
#define UART_FR_RXFE 0x10  // Receive FIFO Empty

volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;
void uart_put_char(char c);
char uart_get_char();
void uart_put_string(const char *s);
void uart_gets_input(char *buffer, int max_length);

int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);

#endif // OS_H