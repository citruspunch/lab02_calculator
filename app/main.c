

void main() {
    char input1[16];
    char input2[16];
    char result_str[16];
    int num1, num2, sum;

    uart_put_string("Program: Add Two Numbers\n");

    while (1) {
        // Prompt for first number
        uart_put_string("Enter first number: ");
        uart_gets_input(input1, sizeof(input1));
        num1 = uart_atoi(input1);

        // Prompt for second number
        uart_put_string("Enter second number: ");
        uart_gets_input(input2, sizeof(input2));
        num2 = uart_atoi(input2);

        // Calculate sum
        sum = num1 + num2;

        // Convert sum to string
        uart_itoa(sum, result_str);

        // Display the result
        uart_put_string("Sum: ");
        uart_put_string(result_str);
        uart_put_char('\n');
    }
}
