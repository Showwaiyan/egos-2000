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

void reverse(char *str, int len) {
  int left_side = 0;
  int right_side = len - 1;
  while (left_side < right_side) {
    char temp = str[left_side];
    str[left_side] = str[right_side];
    str[right_side] = temp;
    left_side++;
    right_side--;
  }
}

char *llutoa(unsigned long long int num, char *str) {
  int i = 0;
  if (num == 0) {
    str[i] = '0';
    str[++i] = '\0';
    return str;
  }

  while (num != 0) {
    int rem = num % 10;
    str[i++] = rem + '0';
    num = num / 10;
  }

  str[i] = '\0';

  reverse(str, i);
  return str;
}

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
        out[len] = (char)va_arg(
            args, int); // should be char for va_arg, but it will convert to int
        // type smaller than int, such as char, bool and short will conver to
        // int as for performance because of word boundary for mechine (cpu
        // don't fetch data just on byte, mostly 4 byte for 32 bit mechine)
        out[len + 1] = '\0'; 
      } else if (*fmt == 'x') {
        itoa(va_arg(args, int), out + strlen(out), 16);
      } else if (*fmt == 'u') {
        utoa(va_arg(args, int), out + strlen(out), 10);
      } else if (*fmt == 'p') {
        strcat(out, "0x");
        utoa(va_arg(args, int), out + strlen(out), 16);
      } else if (*fmt == 'l' || *fmt == 'L') {
        fmt++;
        if (*fmt == 'l' || *fmt == 'L') {
          fmt++;
          if (*fmt == 'u') {
            llutoa(va_arg(args, unsigned long long int), out + strlen(out));
          }
        }
      }
    }
  }
}

unsigned int format_to_str_len(char *fmt, va_list args) {
  unsigned int len = 0;
  for (; *fmt != '\0'; fmt++) {
    if (*fmt != '%') {
      len++;
    } else {
      fmt++;
      if (*fmt == 'c') {
        len++;
      }
      if (*fmt == 's') {
        len += strlen(va_arg(args,char*));
      }
    }
  }
  return len+1; // +1 for null terminator
}

int printf(const char *format, ...) {
  char buf[512];
  va_list args;
  va_start(args, format);
  va_list args_copy;
  va_copy(args_copy,args);
  format_to_str(buf, format, args);
  va_end(args);
  terminal_write(buf, strlen(buf));

  return 0;
}

/* Uncomment line46 - line57
 * when implementing dynamic memory allocation
 */
extern char __heap_start, __heap_end;
static char *brk = &__heap_start;
char *_sbrk(int size) {
  if (brk + size > (char *)&__heap_end) {
    terminal_write("_sbrk: heap grows too large\r\n", 29);
    return NULL;
  }

  char *old_brk = brk;
  brk += size;
  return old_brk;
}

void test_formate_to_str_len(char *fmt,...) {
  va_list args;
  va_start(args, fmt);
  unsigned int test = format_to_str_len(fmt, args);
  printf("Testing '%s': %d", fmt, test);
}

int main() {
  char *msg = "Hello, World!\n\r";
  terminal_write(msg, 15);

  /* Uncomment this line of code
   * when implementing formatted output
   */
  printf("%s-%d is awesome!\n\r", "egos", 2000);

  printf("%c is character $\n\r", '$');
  printf("%c is character 0\n\r", (char)48);
  printf("%x is integer 1234 in hexadecimal\n\r", 1234);
  printf("%u is the maximum of unsigned int\n\r", (unsigned int)0xFFFFFFFF);
  printf("%p is the hexadecimal address of the hello-world string\n\r", msg);
  printf("%llu is the maximum of unsigned long long\n", 0xFFFFFFFFFFFFFFFFULL);

  // test_formate_to_str_len("Hello%c",'!');
  // test_formate_to_str_len("Hello%s","!World");

  return 0;
}
