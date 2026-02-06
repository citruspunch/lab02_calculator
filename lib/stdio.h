#ifndef STDIO_H
#define STDIO_H

#include "os.h"

// Function to print a formatted string analogous to printf
// Supports: %s (string), %d (integer), %f (float), %c (character)
void PRINT(const char* format, ...);

// Function to read a formatted string analogous to scanf
// Supports: %s (string), %d (integer), %f (float), %c (character)
void READ(const char* format, ...);

#endif // STDIO_H