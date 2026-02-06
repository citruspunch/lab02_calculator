#include "os.h"

// Define UART0 pointer (only once in this file)
volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;

// Function to send a single character via UART
void uart_put_char(char c) {
    // Wait until there is space in the FIFO
    while (UART0[UART_FR / 4] & UART_FR_TXFF);
    UART0[UART_DR / 4] = c;
}

// Function to receive a single character via UART
char uart_get_char() {
    // Wait until data is available
    while (UART0[UART_FR / 4] & UART_FR_RXFE);
    return (char)(UART0[UART_DR / 4] & 0xFF);
}

// Function to send a string via UART
void uart_put_string(const char *s) {
    while (*s) {
        uart_put_char(*s++);
    }
}

// Wait for UART to finish transmitting all pending data
void uart_flush(void) {
    while (UART0[UART_FR / 4] & UART_FR_BUSY);
}

// Function to receive a line of input via UART
void uart_gets_input(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) { // Leave space for null terminator
        c = uart_get_char();
        if (c == '\n' || c == '\r') {
            uart_put_char('\n'); // Echo newline
            break;
        }
        uart_put_char(c); // Echo character
        buffer[i++] = c;
    }
    buffer[i] = '\0'; // Null terminate the string
}

// Simple function to convert string to integer
int uart_atoi(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    return sign * num;
}

// Function to convert integer to string
void uart_itoa(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) { // Reserve space for sign and null terminator
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the string
    int start = 0, end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

// Function to convert string to float
float uart_atof(const char *s) {
    float result = 0.0f;
    float fraction = 0.0f;
    int sign = 1;
    int i = 0;
    int decimal_places = 0;
    int past_decimal = 0;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    } else if (s[i] == '+') {
        i++;
    }

    // Parse number
    while (s[i]) {
        if (s[i] >= '0' && s[i] <= '9') {
            if (past_decimal) {
                fraction = fraction * 10.0f + (s[i] - '0');
                decimal_places++;
            } else {
                result = result * 10.0f + (s[i] - '0');
            }
        } else if (s[i] == '.' && !past_decimal) {
            past_decimal = 1;
        } else {
            break; // Stop at invalid character
        }
        i++;
    }

    // Combine integer and fractional parts
    if (decimal_places > 0) {
        float divisor = 1.0f;
        // Calculate divisor for fractional part (10^decimal_places)
        for (int j = 0; j < decimal_places; j++) {
            divisor *= 10.0f;
        }
        result += fraction / divisor;
    }

    return sign * result;
}

// Function to convert float to string
void uart_ftoa(double num, char *buffer, int precision) {
    int i = 0;
    int int_part;
    double fractional_part;

    // Handle negative numbers
    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }

    // Extract integer part
    int_part = (int)num;
    fractional_part = num - int_part;

    // Convert integer part
    char temp[16];
    int temp_i = 0;

    if (int_part == 0) {
        temp[temp_i++] = '0';
    } else {
        while (int_part > 0) {
            temp[temp_i++] = '0' + (int_part % 10);
            int_part /= 10;
        }
    }

    // Reverse and copy integer part
    while (temp_i > 0) {
        buffer[i++] = temp[--temp_i];
    }

    // Add decimal point
    buffer[i++] = '.';

    // Convert fractional part
    for (int j = 0; j < precision; j++) {
        fractional_part *= 10.0;
        int digit = (int)fractional_part;
        buffer[i++] = '0' + digit;
        fractional_part -= digit;
    }

    buffer[i] = '\0';
}