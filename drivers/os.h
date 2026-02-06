#ifndef OS_H
#define OS_H

#define UART0_BASE 0x101f1000

#define UART_DR      0x00  // Data Register
#define UART_FR      0x18  // Flag Register
#define UART_FR_TXFF 0x20  // Transmit FIFO Full
#define UART_FR_RXFE 0x10  // Receive FIFO Empty
#define UART_FR_BUSY 0x08  // UART Busy transmitting

extern volatile unsigned int * const UART0;

// Basic UART operations
void uart_put_char(char c);
char uart_get_char();
void uart_put_string(const char *s);
void uart_gets_input(char *buffer, int max_length);
void uart_flush(void);  // Wait for all pending transmissions

// Conversion functions
int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);
float uart_atof(const char *s);
void uart_ftoa(double num, char *buffer, int precision);

#endif // OS_H