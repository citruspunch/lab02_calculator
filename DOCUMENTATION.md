# Project Documentation

**Layer Separation**

- **Boot / Startup**: (startup assembly and linker) Responsible for CPU setup, stack and vector initialization and transferring control to `main`. This layer performs minimal early hardware setup and then jumps into the runtime provided by the drivers and libraries.

- **Drivers (Hardware Abstraction)**: Located in `drivers/` (notably `os.c` / `os.h`). This layer provides direct access to the UART hardware and low-level conversions. Functions here read/write UART registers and implement safe, tiny conversions between strings and numeric types. Drivers should expose simple, stable primitives that higher layers call.

- **Libraries (Platform C API)**: Located in `lib/` (`stdio.c` / `stdio.h`, `string.c` / `string.h`). This layer implements higher-level I/O and utility functions built on top of the drivers. It handles formatting, parsing of input/output, and small string utilities. It keeps application code simple by exposing `PRINT` and `READ` interfaces.

- **Application**: Located in `app/` (`main.c`). Uses library APIs to implement program logic (prompts, arithmetic, loop). The application should not access UART registers directly — it uses the library/driver abstractions.

**Design rationale**

- Keep hardware-specific code (register accesses, busy-wait loops) in the drivers layer so that it is isolated and easy to replace if the UART base address or platform changes.
- Keep formatting/parsing in the library layer so application code remains readable and compact.

**Function descriptions by layer**

Drivers (`drivers/os.h` / `drivers/os.c`)

- `volatile unsigned int * const UART0`:
  - Pointer mapping to the UART0 base address. All register accesses go through this pointer.

- `void uart_put_char(char c)`:
  - Waits until UART transmit FIFO has space, then writes the character to the data register. Used for sending single bytes.

- `char uart_get_char()`:
  - Waits until the receive FIFO has data, then reads and returns one byte from the UART data register.

- `void uart_put_string(const char *s)`:
  - Iterates the given NUL-terminated string and calls `uart_put_char` for each character. Provides a convenient way to send C strings.

- `void uart_gets_input(char *buffer, int max_length)`:
  - Reads characters from UART (using `uart_get_char`) into `buffer` until newline or until `max_length-1` characters have been read. Echoes characters back and NUL-terminates the buffer. Intended for line-oriented input.

- `void uart_flush(void)`:
  - Polls the UART flag register until the UART is no longer busy, ensuring all pending bytes have been transmitted before continuing.

- `int uart_atoi(const char *s)`:
  - Minimal ASCII-to-integer converter. Handles optional leading `-` sign and consumes consecutive ASCII digits to build an `int`. Stops at the first non-digit.

- `void uart_itoa(int num, char *buffer)`:
  - Converts an `int` to a NUL-terminated ASCII string and writes it into `buffer`. Handles zero and negative numbers; reverses the generated digits into the final string.

- `float uart_atof(const char *s)`:
  - Minimal ASCII-to-float converter. Handles optional sign, integer and fractional parts separated by a `.`. Accumulates integer and fractional digits and returns a `float` approximation.

- `void uart_ftoa(double num, char *buffer, int precision)`:
  - Converts a floating-point value to ASCII in `buffer` with the requested decimal `precision`. Handles negative numbers, integer extraction, and fractional digits by repeated multiplication. Produces no rounding beyond truncation of computed digits.

Libraries (`lib/stdio.h` / `lib/stdio.c`, `lib/string.*`)

- `void PRINT(const char* format, ...)`:
  - A tiny `printf`-like function. Walks the format string and, for supported conversion specifiers, extracts variadic arguments and emits them via the driver primitives. Supported specifiers: `%s` (string), `%d` (int), `%f` (float/double), `%c` (char), and literal `%%`.
  - Implementation notes/limits: uses a local `buffer[32]` for numeric-to-string results; `%f` uses `uart_ftoa` with a hard-coded 2-decimal output. Does not support width/precision flags, exponential notation, or length modifiers.

- `void READ(const char* format, ...)`:
  - A tiny `scanf`-like routine. Flushes UART to ensure prompts are visible, reads a whole input line into an internal buffer with `uart_gets_input`, then parses that buffer according to the provided `format` string. Supported specifiers: `%s`, `%d`, `%f`, `%c`.
  - Implementation notes/limits: tokenizes based on whitespace and simple character matching. Numeric parsing delegates to `uart_atoi` and `uart_atof`. Buffers are fixed-size (`input_buffer[64]`) — overly long input may be truncated.

- `char *my_strncpy(char *dest, const char *src, size_t n)` (in `lib/string.c`):
  - Minimal `strncpy` replacement: copies up to `n` characters from `src` into `dest`. If `src` is shorter than `n`, pads the remainder with NUL bytes. Returns `dest`.

Application (`app/main.c`)

- `void main()`:
  - Demonstrates use of the library and driver abstractions. It runs an infinite loop that:
    - Prompts for two integers with `PRINT`/`READ`, computes their sum, and prints the result.
    - Prompts for two floats with `PRINT`/`READ`, computes their sum, and prints the result.
  - The application code never touches UART registers directly — it relies on `PRINT`/`READ` which call into the driver layer.

**Notes, limitations, and suggested improvements**

- Buffer sizes are small (e.g., `buffer[32]` in `PRINT`, `input_buffer[64]` in `READ`). Increase them if you expect longer strings or numeric output.
- `PRINT` and `READ` implement a useful subset but lack full `printf`/`scanf` semantics (no width/precision flags, no error reporting). If needed, extend format parsing gradually.
- Floating conversion in `uart_ftoa` truncates rather than rounds; adjust algorithm if correct rounding is required.
- Consider adding non-blocking or interrupt-driven UART handling for better responsiveness and to avoid busy-wait polling in production.

---

File: [app/main.c](app/main.c)
