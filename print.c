/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: code in this file is copy-pasted from P0.
 * You can copy-paste your P0 solution code into this file.
 */

void terminal_write(const char *str, int len) {
    for (int i = 0; i < len; i++) {
        *(char*)(0x10000000UL) = str[i];
    }
}

#include <stdlib.h>  // for itoa() and utoa()
#include <string.h>  // for strlen() and strcat()
#include <stdarg.h>  // for va_start(), va_end(), va_arg() and va_copy()

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

unsigned int len_itoa(int value, int base) {
  unsigned int len = 0;
  if (value == 0)
    return 1; // zero is one digit
  int num = value;
  if (value < 0) {
    len++; // '-' sign is one length
    num = (unsigned int)(-(int)value);
  };
  while (num != 0) {
    len++;
    num /= base;
  }
  return len;
}

unsigned int len_utoa(unsigned int value, int base) {
  unsigned int len = 0;
  if (value == 0)
    return 1; // zero is one digit
  unsigned int num = value;
  while (num != 0) {
    len++;
    num /= base;
  }
  return len;
}

unsigned int len_llutoa(unsigned long long value, int base) {
  unsigned int len = 0;
  if (value == 0)
    return 1; // zero is one digit
  unsigned long long num = value;
  while (num != 0) {
    len++;
    num /= base;
  }
  return len;
}

unsigned int format_to_str_len(const char *fmt, va_list args) {
  unsigned int len = 1; // +1 for \0
  for (; *fmt != '\0'; fmt++) {
    if (*fmt != '%') {
      len++;
    } else {
      fmt++;
      if (*fmt == 'c') {
        len++;
      } else if (*fmt == 's') {
        len += strlen(va_arg(args, char *));
      } else if (*fmt == 'd') {
        len += len_itoa(va_arg(args, int), 10);
      } else if (*fmt == 'x') {
        len += len_itoa(va_arg(args, int), 16);
      } else if (*fmt == 'u') {
        len += len_utoa(va_arg(args, unsigned int), 10);
      } else if (*fmt == 'p') {
        len += 2; // for "0x"
        len += len_utoa(va_arg(args, unsigned int), 16);
      } else if (*fmt == 'l' || *fmt == 'L') {
        fmt++;
        if (*fmt == 'l' || *fmt == 'L') {
          fmt++;
          if (*fmt == 'u') {
            len += len_llutoa(va_arg(args, unsigned long long), 10);
          }
        }
      }
    }
  }
  return len;
}

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  va_list args_copy;
  va_copy(args_copy, args);
  unsigned int len = format_to_str_len(format, args_copy);
  char *buf = malloc(len);
  format_to_str(buf, format, args);
  va_end(args);
  va_end(args_copy);
  terminal_write(buf, strlen(buf));
  free(buf);

  return 0;
}

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
