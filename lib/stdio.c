#include "stdio.h"
#include <stdarg.h>

// Function to print a formatted string analogous to printf
// Supports: %s (string), %d (integer), %f (float), %c (character)
void PRINT(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[32];
    const char *ptr = format;
    
    while (*ptr) {
        if (*ptr == '%' && *(ptr + 1)) {
            ptr++; // Move past '%'
            
            switch (*ptr) {
                case 's': {
                    // String
                    char *str = va_arg(args, char*);
                    uart_put_string(str);
                    break;
                }
                case 'd': {
                    // Integer
                    int num = va_arg(args, int);
                    uart_itoa(num, buffer);
                    uart_put_string(buffer);
                    break;
                }
                case 'f': {
                    // Float
                    double num = va_arg(args, double);
                    uart_ftoa(num, buffer, 2); // 2 decimal places
                    uart_put_string(buffer);
                    break;
                }
                case 'c': {
                    // Character
                    char c = (char)va_arg(args, int);
                    uart_put_char(c);
                    break;
                }
                case '%': {
                    // Literal '%'
                    uart_put_char('%');
                    break;
                }
                default:
                    // Unknown specifier, print as-is
                    uart_put_char('%');
                    uart_put_char(*ptr);
                    break;
            }
        } else {
            // Regular character
            uart_put_char(*ptr);
        }
        ptr++;
    }
    
    va_end(args);
}

// Function to read a formatted string analogous to scanf
// Supports: %s (string), %d (integer), %f (float), %c (character)
void READ(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char input_buffer[64];
    const char *ptr = format;
    int input_index = 0;
    
    // Flush UART to ensure prompt is displayed before waiting for input
    uart_flush();
    
    // Read entire line of input
    uart_gets_input(input_buffer, sizeof(input_buffer));
    
    while (*ptr && input_buffer[input_index]) {
        if (*ptr == '%' && *(ptr + 1)) {
            ptr++;
            
            // Skip whitespace in input
            while (input_buffer[input_index] == ' ' || input_buffer[input_index] == '\t') {
                input_index++;
            }
            
            switch (*ptr) {
                case 's': {
                    // String - read until whitespace or end
                    char *str = va_arg(args, char*);
                    int i = 0;
                    while (input_buffer[input_index] && 
                           input_buffer[input_index] != ' ' && 
                           input_buffer[input_index] != '\t' &&
                           input_buffer[input_index] != '\n') {
                        str[i++] = input_buffer[input_index++];
                    }
                    str[i] = '\0';
                    break;
                }
                case 'd': {
                    // Integer
                    int *num_ptr = va_arg(args, int*);
                    char temp[16];
                    int i = 0;
                    
                    // Handle negative sign
                    if (input_buffer[input_index] == '-') {
                        temp[i++] = input_buffer[input_index++];
                    }
                    
                    // Read digits
                    while (input_buffer[input_index] >= '0' && input_buffer[input_index] <= '9') {
                        temp[i++] = input_buffer[input_index++];
                    }
                    temp[i] = '\0';
                    *num_ptr = uart_atoi(temp);
                    break;
                }
                case 'f': {
                    // Float
                    float *num_ptr = va_arg(args, float*);
                    char temp[32];
                    int i = 0;
                    
                    // Handle negative sign
                    if (input_buffer[input_index] == '-') {
                        temp[i++] = input_buffer[input_index++];
                    }
                    
                    // Read digits and decimal point
                    while ((input_buffer[input_index] >= '0' && input_buffer[input_index] <= '9') ||
                           input_buffer[input_index] == '.') {
                        temp[i++] = input_buffer[input_index++];
                    }
                    temp[i] = '\0';
                    *num_ptr = uart_atof(temp);
                    break;
                }
                case 'c': {
                    // Character
                    char *c = va_arg(args, char*);
                    *c = input_buffer[input_index++];
                    break;
                }
                default:
                    break;
            }
        } else if (*ptr == ' ' || *ptr == '\t') {
            // Skip whitespace in format string
            while (input_buffer[input_index] == ' ' || input_buffer[input_index] == '\t') {
                input_index++;
            }
        } else {
            // Match literal character
            if (input_buffer[input_index] == *ptr) {
                input_index++;
            }
        }
        ptr++;
    }
    
    va_end(args);
}