#include "stdio.h"

// Function to print a formatted string analogous to printf
void PRINT() {
    uart_put_string(buffer);
}

// Function to read a formatted string analogous to scanf
void READ(char* buffer, int length) {
    uart_gets_input(buffer, length);
}