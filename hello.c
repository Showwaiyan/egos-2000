void terminal_write(const char *str, int len) {
  for (int i = 0; i < len; i++) {
    *(char *)(0x10000000) = str[i];
  }
}

/* Uncomment line11 - line39
 * when implementing formatted output
 */
#include <stdarg.h> // for va_start(), va_end(), va_arg() and va_copy()
#include <stdlib.h> // for itoa() and utoa()
#include <string.h> // for strlen() and strcat()

void format_to_str(char *out, const char *fmt, va_list args) {
  for (out[0] = 0; *fmt != '\0'; fmt++) {
    if (*fmt != '%') {
      strncat(out, fmt, 1);
    } else {
      fmt++;
      if (*fmt == 's') {
        strcat(out, va_arg(args, char *));
      } else if (*fmt == 'd') {
        itoa(va_arg(args, int), out + strlen(out), 10);
      } else if (*fmt == 'c') {
        size_t len = strlen(out);
        out[len] = (char)va_arg(args,int); // should be char for va_arg, but it will convert to int
        // type smaller than int, such as char, bool and short will conver to int as for performance
        // because of word boundary for mechine (cpu don't fetch data just on byte, mostly 4 byte for 32 bit mechine)
        out[len+1] = '\0';
      }
    }
  }
}

int printf(const char *format, ...) {
  char buf[512];
  va_list args;
  va_start(args, format);
  format_to_str(buf, format, args);
  va_end(args);
  terminal_write(buf, strlen(buf));

  return 0;
}

/* Uncomment line46 - line57
 * when implementing dynamic memory allocation
 */
/*
extern char __heap_start, __heap_end;
static char* brk = &__heap_start;
char* _sbrk(int size) {
    if (brk + size > (char*)&__heap_end) {
        terminal_write("_sbrk: heap grows too large\r\n", 29);
        return NULL;
    }

    char* old_brk = brk;
    brk += size;
    return old_brk;
}
*/

int main() {
  char *msg = "Hello, World!\n\r";
  terminal_write(msg, 15);

  /* Uncomment this line of code
   * when implementing formatted output
   */
  printf("%s-%d is awesome!\n\r", "egos", 2000);

  printf("%c is character $\n\r", '$');
  // printf("%c is character 0", (char)48);
  // printf("%x is integer 1234 in hexadecimal", 1234);
  // printf("%u is the maximum of unsigned int", (unsigned int)0xFFFFFFFF);
  // printf("%p is the hexadecimal address of the hello-world string", msg);
  // printf("%llu is the maximum of unsigned long long", 0xFFFFFFFFFFFFFFFFULL);
  return 0;
}
