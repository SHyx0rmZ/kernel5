/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込
 *  Copyright (C) 2010 Patrick Pokatilo
 *
 *  This file is part of Nuke (理コ込).
 *
 *  Nuke (理コ込) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nuke (理コ込) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nuke (理コ込).  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "console.h"
#include "io.h"

static short *video = (short *)0xb8000;
static short color = 0x0700;
static int printed = 0;
static char converted[256];
static char intermediate[256];

/* convert a number to a string, save result in converted */
void convert_number(uintmax_t number, uint8_t radix, char pad, uintmax_t width, uintmax_t length, uintmax_t precision, bool uppercase, uint8_t sign)
{
    uint8_t negative = 0;

    /* use signs */
    if (sign & 4)
    {
        negative = 4;
    }
    else if (sign & 2)
    {
        negative = 2;
    }

    /* char */
    if (UNLIKELY(length == 2))
    {
        number &= (char)-1;

        /* if signed, *= -1 */
        if ((sign & 1) && (number >= (1ULL << ((8 * sizeof(char)) - 1))))
        {
            number = (~number) + 1;
            number &= (char)-1;

            negative |= 1;
        }
    }
    /* short */
    else if (UNLIKELY(length == 1))
    {
        number &= (short)-1;

        /* if signed, *= -1 */
        if ((sign & 1) && (number >= (1ULL << ((8 * sizeof(short)) - 1))))
        {
            number = (~number) + 1;
            number &= (short)-1;

            negative |= 1;
        }
    }
    /* int */
    else if (LIKELY(length == 0))
    {
        number &= (int)-1;

        /* if signed, *= -1 */
        if ((sign & 1) && (number >= (1ULL << ((8 * sizeof(int)) - 1))))
        {
            number = (~number) + 1;
            number &= (int)-1;

            negative |= 1;
        }
    }
    /* long */
    else if (LIKELY(length == 3))
    {
        number &= (long)-1;

        /* if signed, *= -1 */
        if ((sign & 1) && (number >= (1ULL << ((8 * sizeof(long)) - 1))))
        {
            number = (~number) + 1;
            number &= (long)-1;

            negative |= 1;
        }
    }
    /* long long */
    else
    {
        number &= (long long)-1;

        /* if signed, *= -1 */
        if ((sign & 1) && (number >= (1ULL << ((8 * sizeof(long long)) - 1))))
        {
            number = (~number) + 1;
            number &= (long long)-1;

            negative |= 1;
        }
    }

    uint8_t index = 0;

    /* convert number to string */
    while ((LIKELY(number)) && (LIKELY(index < 255)))
    {
        /* get digit */
        uint8_t digit = number % radix;

        number /= radix;

        /* convert digit to char */
        if (UNLIKELY(digit >= 10))
        {
            if (UNLIKELY(uppercase))
            {
                intermediate[index] = digit + 'A' - 10;
            }
            else
            {
                intermediate[index] = digit + 'a' - 10;
            }
        }
        else
        {
            intermediate[index] = digit + '0';
        }

        index++;
    }

    /* handle special case */
    if (UNLIKELY(index == 0))
    {
        intermediate[index] = '0';

        index++;
    }

    /* add sign */
    if (negative && ((index >= width) || (index >= precision) || (pad == ' ')))
    {
        if (negative & 1)
        {
            intermediate[index] = '-';
        }
        else if (negative & 4)
        {
            intermediate[index] = ' ';
        }
        else
        {
            intermediate[index] = '+';
        }

        negative = 0;

        index++;
    }

    /* pad */
    while (((LIKELY(index < width)) || (UNLIKELY(index < precision))) && (LIKELY(index < 255)))
    {
        intermediate[index] = pad;

        index++;
    }

    index--;

    /* add sign */
    if (negative)
    {
        if (negative & 1)
        {
            intermediate[index] = '-';
        }
        else
        {
            intermediate[index] = '+';
        }
    }

    /* reverse string */
    for (width = 0; LIKELY(index != (uint8_t)-1); index--, width++)
    {
        converted[width] = intermediate[index];
    }

    /* add null byte */
    converted[width] = 0;
}

void putc(const char c)
{
    /* reset caret to beginning of line */
    if (UNLIKELY(c == '\r'))
    {
        video = (short *)((uintptr_t)video - (((uintptr_t)video - 0xb8000) % 160));
    }
    /* reset caret to beginning of next line */
    else if (UNLIKELY(c == '\n'))
    {
        video = (short *)((uintptr_t)video - (((uintptr_t)video - 0xb8000) %160) + 160);
    }
    else
    {
        /* scroll lines */
        while (UNLIKELY((uintptr_t)video >= (0xb8000 + (160 * 25))))
        {
            memmove((void *)0xb8000, (void *)(0xb8000 + 160), 160 * 24);
            memset((void *)(0xb8000 + (160 * 24)), 0, 160);

            video = (short *)((uintptr_t)video - 160);
        }

        *(video++) = color | c;
    }

#ifdef PRINT_DEBUG
    outb(0xe9, c);
    while((inb(0x3fd) & 0x20) == 0) __asm__ ("nop");
    outb(0x3f8, c);
#endif

    printed++;
}

void puts(const char *s, uintmax_t n)
{
    while ((LIKELY(*s)) && (LIKELY(n > 0)))
    {
        putc(*s);

        s++;
        n--;
    }
}

int printf(const char *format, ...)
{
    printed = 0;

    va_list args;

    /* initialize variable argument list */
    va_start(args, format);

    while (LIKELY(*format))
    {
        if (UNLIKELY(*format == '%'))
        {
            format++;

            /* not in standard, use this for color */
            if (UNLIKELY(*format == '['))
            {
                color = va_arg(args, int) << 8;

                format++;

                continue;
            }
            /* not in standard, reset to default color */
            else if (UNLIKELY(*format == ']'))
            {
                color = 0x0700;

                format++;

                continue;
            }

            uintmax_t width = 0;
            char pad = ' ';
            char pad_mod = 0;
            uintmax_t length = 0; /* int */
            uintmax_t precision = 0;
            bool alternate = false;
            uint8_t sign = 0;

            /* use alternate forms for o, x, X, a, A, e, E, f, F, g and G */
            if (*format == '#')
            {
                alternate = true;

                format++;
            }

            /* pad with zero */
            if (*format == '0')
            {
                pad = '0';

                pad_mod++;
                format++;
            }

            /* pad positives with space */
            if (*format == ' ')
            {
                sign = 4;

                pad_mod++;
                format++;
            }

            /* always add sign */
            if (*format == '+')
            {
                sign = 2;

                format++;
            }

            /* get width */
            while ((*format >= '0') && (*format <= '9'))
            {
                width = (width * 10) + (*format - '0');

                format++;
            }

            /* get precision */
            if (UNLIKELY(*format == '.'))
            {
                format++;

                while ((*format >= '0') && (*format <= '9'))
                {
                    precision = (precision * 10) + (*format - '0');

                    format++;
                }
            }

            /* get length */
            if (UNLIKELY(*format == 'h'))
            {
                /* short */
                length = 1;

                format++;

                if (LIKELY(*format == 'h'))
                {
                    /* char */
                    length = 2;

                    format++;
                }
            }
            else if (UNLIKELY(*format == 'l'))
            {
                /* long */
                length = 3;

                format++;

                if (LIKELY(*format == 'l'))
                {
                    /* long long */
                    length = 4;

                    format++;
                }
            }

            /* signed int */
            if ((UNLIKELY(*format == 'd')) || (UNLIKELY(*format == 'i')))
            {
                uintmax_t arg = 0;

                /* get argument, char and short are passed as int */
                if (UNLIKELY(length == 3))
                {
                    arg = va_arg(args, long int);
                }
                else if (UNLIKELY(length == 4))
                {
                    arg = va_arg(args, long long int);
                }
                else
                {
                    arg = va_arg(args, int);
                }

                sign++;

                /* convert number */
                convert_number(arg, 10, pad, width, length, precision, false, sign);

                /* and print it */
                puts(converted, -1);

                format++;
            }
            /* unsigned int */
            else if ((UNLIKELY(*format == 'o')) || (LIKELY(*format == 'u')) || (LIKELY(*format == 'x')) || (UNLIKELY(*format == 'X')))
            {
                uintmax_t arg = 0;

                /* get argument, char and short are passed as int */
                if (UNLIKELY(length == 3))
                {
                    arg = va_arg(args, unsigned long int);
                }
                else if (UNLIKELY(length == 4))
                {
                    arg = va_arg(args, unsigned long long int);
                }
                else
                {
                    arg = va_arg(args, unsigned int);
                }

                if (UNLIKELY(*format == 'o'))
                {
                    if (alternate)
                    {
                        puts("0", 1);

                        if (width > 1)
                        {
                            width -= 1;
                        }
                    }

                    /* convert number */
                    convert_number(arg, 8, pad, width, length, precision, false, sign);
                }
                else if (LIKELY(*format == 'u'))
                {
                    /* convert number */
                    convert_number(arg, 10, pad, width, length, precision, false, sign);
                }
                else if (LIKELY(*format == 'x'))
                {
                    if (alternate)
                    {
                        puts("0x", 2);

                        if (width > 1)
                        {
                            width -= 2;
                        }
                    }

                    /* convert number */
                    convert_number(arg, 16, pad, width, length, precision, false, sign);
                }
                else
                {
                    if (alternate)
                    {
                        puts("0X", 2);

                        if (width > 1)
                        {
                            width -= 2;
                        }
                    }

                    /* convert number */
                    convert_number(arg, 16, pad, width, length, precision, true, sign);
                }

                /* and print it */
                puts(converted, -1);

                format++;
            }
            /* const char * */
            else if (LIKELY(*format == 's'))
            {
                const char *arg = va_arg(args, const char *);

                if (LIKELY(precision == 0))
                {
                    precision = -1;
                }

                puts(arg, precision);

                format++;
            }
            /* unsigned char */
            else if (UNLIKELY(*format == 'c'))
            {
                unsigned char arg = va_arg(args, int);

                putc(arg);

                format++;
            }
            /* void * */
            else if (LIKELY(*format == 'p'))
            {
                void *arg = va_arg(args, void *);

                /* handle NULL */
                if (UNLIKELY(arg == NULL))
                {
                    puts("(nil)", 5);
                }
                else
                {
                    /* use default formatting for %p */
                    puts("0x", 2);

                    if (LIKELY(width == 0))
                    {
                        width = 2 * sizeof(void *);
                    }
                    else
                    {
                        width -= 2;
                    }

                    if (LIKELY(pad_mod == 0))
                    {
                        pad = '0';
                    }

                    /* convert number */
                    convert_number((uintmax_t)(uintptr_t)arg, 16, pad, width, length, precision, false, sign);

                    /* and print it */
                    puts(converted, -1);
                }

                format++;
            }
            /* return number of bytes written so far */
            else if (UNLIKELY(*format == 'n'))
            {
                int *arg = va_arg(args, int *);

                if (arg != NULL)
                {
                    *arg = printed;
                }

                format++;
            }
            /* pad with char until width */
            else if (UNLIKELY(*format == 'P'))
            {
                char arg = va_arg(args, int);

                if (width > (uintmax_t)printed)
                {
                    width = width - printed;

                    while (LIKELY(width > 0))
                    {
                        putc(arg);

                        width--;
                    }
                }

                format++;
            }
            else /* if(*format == '%') */
            {
                puts("%", 1);

                if (LIKELY(*format == '%'))
                {
                    format++;
                }
            }
        }
        else
        {
            /* just put char */
            putc(*format);

            format++;
        }
    }

    va_end(args);

    return printed;
}
