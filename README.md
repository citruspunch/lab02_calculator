# Project Documentation

**Separación por capas**
- **OS level**: Ubicada en el folder `drivers/` que incluye `os.c`, `os.h`. Esta capa es la que proporciona acceso directo al hardware (via UART) y también contiene conversiones de tipo. Las funciones aquí leen/escriben los registros del UART e implementan conversiones entre cadenas y tipos numéricos. Los drivers exponen funciones simples y estables que las capas superiores llaman.

- **Language Libraries**: Ubicada en `lib/` que incluye `stdio.c`, `stdio.h`, `string.c` , `string.h`. Esta capa implementa funciones de IO y utilidades de formato/parsing utilizando los drivers. Mantiene el código de la aplicación simple al exponer interfaces como `PRINT` y `READ`.

- **User level**: Ubicada en `app/` (`main.c`). Usa las APIs de la librería `stdio` creadas en la capa `lib/` para implementar la lógica del programa. La aplicación no debe acceder directamente a los registros UART, utiliza las abstracciones de la biblioteca.


**Diseño**

- El código de bajo nivel (los drivers) se mantiene separado del código de alto nivel (aplicación) a través de la capa de librería, lo que permite que la lógica de la aplicación sea independiente del hardware específico y facilita el mantenimiento y la portabilidad.
- La capa de librería se encarga de formatear/parsing, lo que mantiene el código de la aplicación limpio y enfocado en la lógica de negocio, sin preocuparse por los detalles de cómo se envían o reciben los datos a través del UART.


**Function descriptions by layer**

Drivers (`drivers/os.h` / `drivers/os.c`)

- `volatile unsigned int * const UART0`:
  - Puntero constante a la dirección base del UART0. Todas las lecturas/escrituras a los registros del UART se hacen a través de este puntero. 

- `void uart_put_char(char c)`:
  - Envía un solo carácter a través del UART.

- `char uart_get_char()`:
  - Obtiene un byte del UART. Espera hasta que el FIFO de recepción tenga datos, luego lee y devuelve un byte del registro de datos del UART.

- `void uart_put_string(const char *s)`:
  - Envia strings. Itera la cadena terminada en NUL dada y llama a `uart_put_char` para cada carácter. 

- `void uart_gets_input(char *buffer, int max_length)`:
  - Lee caracteres del UART usando `uart_get_char` en `buffer` hasta un salto de línea o hasta que se hayan leído `max_length-1` caracteres. Hace eco de los caracteres y termina el buffer con `\0`. 

- `void uart_flush(void)`:
  - Consulta el registro de estado del UART hasta que el UART ya no esté ocupado, para que todos los bytes pendientes hayan sido transmitidos antes de continuar.

- `int uart_atoi(const char *s)`:
  - Convierte de ASCII a número entero. Maneja negativos (opcionales) al inicio y lee dígitos ASCII consecutivos para construir un `int`. Se detiene en el primer carácter que no es número.

- `void uart_itoa(int num, char *buffer)`:
  - Convierte un `int` a una cadena ASCII terminada en `\0` y la escribe en `buffer`. Maneja cero y números negativos; invierte los dígitos generados en la cadena final.

- `float uart_atof(const char *s)`:
  - Convierte ASCII a flotante. Maneja signo opcional, partes enteras y fraccionarias separadas por un `.`. Devuelve una aproximación en `float`.

- `void uart_ftoa(double num, char *buffer, int precision)`:
  - Convierte un valor de punto flotante a ASCII en `buffer` con la precisión decimal solicitada. Maneja números negativos.
  

Librerias (`lib/stdio.h` / `lib/stdio.c`, `lib/string.*`)

- `void PRINT(const char* format, ...)`:
  - Una función parecida a `printf`. Recorre el string de formato y extrae los argumentos pasados y los emite usando las funciones provistas por el driver. Soporta: `%s` (string), `%d` (int), `%f` (float/double), `%c` (char) y `%%` literal.

- `void READ(const char* format, ...)`:
  - Es una función parecida a `scanf`. Vacía el UART para asegurar que los prompts sean visibles, lee una línea completa de entrada en un buffer interno con `uart_gets_input`, luego analiza ese buffer según el string de formato proporcionado. Soporta: `%s`, `%d`, `%f`, `%c`.
  - Limitaciones: No puede leer cadenas que contengan espacios (usa el espacio como delimitador), no maneja errores de formato, y no soporta precisión dinámica.

- `char *my_strncpy(char *dest, const char *src, size_t n)` (in `lib/string.c`):
  - Reemplazo mínimo de `strncpy`, que copia hasta `n` caracteres de `src` a `dest`.


Application (`app/main.c`)

- `void main()`:
  - Utiliza el script para testear la funcionalidad provisto por el Ing. Etson.
