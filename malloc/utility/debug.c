#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

static void write_char(int c);
static void function_printf(void (*char_send)(int), const char *fmt, va_list v);

void mem_safe_printf(const char *fmt, ...)
{
  va_list v;
  va_start(v, fmt);
  function_printf(&write_char, fmt, v);
  va_end(v);
}

static void function_printf(void (*char_send)(int), const char *fmt, va_list v)
{
  const char *digits = "0123456789abcdef";

  enum printf_flags {
    P_ALTERNATE  = 0x001,
    P_ZERO_PAD   = 0x002,
    P_LEFT_ALIGN = 0x004,
    P_SPACE_SIGN = 0x008,
    P_PLUS_SIGN  = 0x010,
    P_PREC_ARG   = 0x020,
    P_HEX_UPPER  = 0x040,
    P_HAVE_WIDTH = 0x100,
    P_HAVE_PREC  = 0x200,
  };
  const char *p, *q;

  for(p = fmt; *p; p++) {
    if(*p != '%') {
      char_send(*p);
      continue;
    }

    int flags = 0;
    int width = 0, prec = 0;
    int lh_mod = 2;

    for(q = p+1; *q; q++) {
      if(*q == '#')
        flags |= P_ALTERNATE;
      else if(*q == '0')
        flags |= P_ZERO_PAD;
      else if(*q == '-')
        flags |= P_LEFT_ALIGN;
      else if(*q == ' ')
        flags |= P_SPACE_SIGN;
      else if(*q == '+')
        flags |= P_PLUS_SIGN;
      else
        break;
    }

    if(flags & P_PLUS_SIGN && flags & P_SPACE_SIGN)
      flags &= ~P_SPACE_SIGN;
    if(flags & P_ZERO_PAD && flags & P_LEFT_ALIGN)
      flags &= ~P_ZERO_PAD;

    if(*q > '0' && *q <= '9') {
      flags |= P_HAVE_WIDTH;
      for(; *q >= '0' && *q <= '9'; q++)
        width = (width * 10) + (*q - '0');
    }

    if(*q == '.') {
      ++q;
      flags |= P_HAVE_PREC;
      if(*q >= '0' && *q <= '9') {
        for(; *q >= '0' && *q <= '9'; q++)
          prec = (prec * 10) + (*q - '0');
      } else if(*q == '*') {
        flags |= P_PREC_ARG;
        ++q;
      }
    }

    for(; *q == 'h'; q++)
      --lh_mod;
    for(; *q == 'l'; q++)
      ++lh_mod;

    switch(*q) {
    case 'i':
    case 'd':
      {
        long long k;
        if(lh_mod >= 4)
          k = va_arg(v, long long);
        else if(lh_mod == 3)
          k = va_arg(v, long);
        else if(lh_mod == 2)
          k = va_arg(v, int);
        else if(lh_mod == 1)
          k = va_arg(v, int);
        else
          k = va_arg(v, int);

        char sign;
        if(k < 0) {
          sign = '-';
          k = -k;
        } else {
          if(flags & P_SPACE_SIGN)
            sign = ' ';
          else if(flags & P_PLUS_SIGN)
            sign = '+';
          else
            sign = 0;
        }

        if(sign) --width;

        char buf[16];
        char *r = &buf[15];
        if(k) {
          for(; k; r--) {
            *r = (k % 10) + '0';
            k = k / 10;
            --width;
          }
        } else {
          *r-- = '0';
          --width;
        }

        char pad_char;
        if(flags & P_HAVE_WIDTH) {
          if(flags & P_ZERO_PAD)
            pad_char = '0';
          else
            pad_char = ' ';
        }

        if(!(flags & P_LEFT_ALIGN) && flags & P_HAVE_WIDTH) {
          for(; width > 0; width--)
            char_send(pad_char);
        }

        if(sign)
          char_send(sign);
        for(++r; r < buf+16; r++)
          char_send(*r);

        if(flags & P_LEFT_ALIGN && flags & P_HAVE_WIDTH) {
          for(; width > 0; width--)
            char_send(pad_char);
        }
      }
      break;

    case 'X':
      flags |= P_HEX_UPPER;
    case 'p':
    case 'x':
      {
        unsigned long long k;
        if(lh_mod >= 4)
          k = va_arg(v, unsigned long long);
        else if(lh_mod == 3)
          k = va_arg(v, unsigned long);
        else if(lh_mod == 2)
          k = va_arg(v, unsigned int);
        else if(lh_mod == 1)
          k = va_arg(v, unsigned int);
        else
          k = va_arg(v, unsigned int);

        int fnzd;
        for(fnzd = 15; fnzd > 0; fnzd--)
          if(k & 0xfll << (4*fnzd))
            break;
        width -= fnzd + 1;

        char pad_char;
        if(flags & P_HAVE_WIDTH) {
          if(flags & P_ZERO_PAD)
            pad_char = '0';
          else
            pad_char = ' ';
        }

        if(flags & P_ALTERNATE)
          width -= 2;

        if(!(flags & P_LEFT_ALIGN) && flags & P_HAVE_WIDTH) {
          for(; width > 0; width--)
            char_send(pad_char);
        }

        if(flags & P_ALTERNATE) {
          char_send('0');
          char_send('x');
        }
        for(; fnzd >= 0; fnzd--)
          char_send(digits[k >> 4*fnzd & 0xf]);

        if(flags & P_LEFT_ALIGN && flags & P_HAVE_WIDTH) {
          for(; width > 0; width--)
            char_send(pad_char);
        }
      }
      break;

    case 'c':
      {
        int k;
        k = va_arg(v, int);
        char_send(k);
      }
      break;

    case 's':
      {
        const char *s;

        if(flags & P_HAVE_PREC && flags & P_PREC_ARG)
          prec = va_arg(v, int);

        s = va_arg(v, const char*);

        int len;
        for(len = 0; s[len]; len++);
        if(flags & P_HAVE_PREC && len > prec)
          len = prec;

        if(flags & P_HAVE_WIDTH) {
          width -= len;
          for(; width > 0; width--)
            char_send(' ');
        }

        int i;
        for(i = 0; i < len; i++)
          char_send(s[i]);
      }
      break;

    case '%':
      char_send('%');
      break;

    default:
      for(; p < q; p++)
        char_send(*p);
    }
    p = q;
  }
}

void write_char(int c)
{
  write(1, &c, 1);
}

