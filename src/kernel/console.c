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
#include <string.h>

#include "console.h"

static short *video = (short *)0xb8000;
static short color = 0x0700;
static int printed = 0;
static char converted[256];
static char intermediate[256];

void convert_number(uintmax_t number, uint8_t radix, char pad, uintmax_t width, uintmax_t precision, bool uppercase, bool sign)
{
    if (precision == 2)
    {
        number &= (char)-1;

        if (sign && (number >= (1ULL << ((8 * sizeof(char)) - 1))))
        {
            number = (~number) + 1;
            number &= (char)-1;
        }
    }
    else if (precision == 1)
    {
        number &= (short)-1;

        if (sign && (number >= (1ULL << ((8 * sizeof(short)) - 1))))
        {
            number = (~number) + 1;
            number &= (short)-1;
        }
    }
    else if (precision == 0)
    {
        number &= (int)-1;

        if (sign && (number >= (1ULL << ((8 * sizeof(int)) - 1))))
        {
            number = (~number) + 1;
            number &= (int)-1;
        }
    }
    else if (precision == 3)
    {
        number &= (long)-1;

        if (sign && (number >= (1ULL << ((8 * sizeof(long)) - 1))))
        {
            number = (~number) + 1;
            number &= (long)-1;
        }
    }
    else
    {
        number &= (long long)-1;

        if (sign && (number >= (1ULL << ((8 * sizeof(long long)) - 1))))
        {
            number = (~number) + 1;
            number &= (long long)-1;
        }
        }

    uint8_t index = 0;

    while (number && index < 255)
    {
        uint8_t digit = number % radix;

        number /= radix;

        if (digit >= 10)
        {
            if (uppercase)
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

    if (index == 0)
    {
        intermediate[index] = '0';

        index++;
    }

    while ((index < width) && (index < 255))
    {
        intermediate[index] = pad;

        index++;
    }

    index--;

    for (width = 0; index != (uint8_t)-1; index--, width++)
    {
        converted[width] = intermediate[index];
    }

    converted[width] = 0;
}

void putc(const char c)
{
    if (c == '\r')
    {
        video = (short *)((uintptr_t)video - (((uintptr_t)video - 0xb8000) % 160));
    }
    else if (c == '\n')
    {
        video = (short *)((uintptr_t)video - (((uintptr_t)video - 0xb8000) %160) + 160);
    }
    else
    {
        if ((uintptr_t)video >= (0xb8000 + (160 * 25)))
        {
            memmove((void *)0xb8000, (void *)(0xb8000 + 160), 160 * 24);
            memset((void *)(0xb8000 + (160 * 24)), 0, 160);

            video = (short *)((uintptr_t)video - 160);
        }

        *(video++) = color | c;
    }

    printed++;
}

void puts(const char *s)
{
    while (*s)
    {
        putc(*s);

        s++;
    }
}

int printf(const char *format, ...)
{
    printed = 0;

    va_list args;

    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            if (*format == '[')
            {
                color = va_arg(args, int) << 8;

                format++;

                continue;
            }
            else if (*format == ']')
            {
                color = 0x0700;

                format++;

                continue;
            }

            uintmax_t width = 0;
            char pad = ' ';
            uintmax_t precision = 0;
            bool alternate = false;

            if (*format == '#')
            {
                alternate = true;

                format++;
            }

            if (*format == '0')
            {
                pad = '0';

                format++;
            }

            while ((*format >= '0') && (*format <= '9'))
            {
                width = (width * 10) + (*format - '0');

                format++;
            }

            if (*format == 'h')
            {
                precision = 1;

                format++;

                if (*format == 'h')
                {
                    precision = 2;

                    format++;
                }
            }
            else if (*format == 'l')
            {
                precision = 3;

                format++;

                if (*format == 'l')
                {
                    precision = 4;

                    format++;
                }
            }

            if ((*format == 'd') || (*format == 'i'))
            {
                uintmax_t arg = 0;

                if (precision == 3)
                {
                    arg = va_arg(args, long int);
                }
                else if (precision == 4)
                {
                    arg = va_arg(args, long long int);
                }
                else
                {
                    arg = va_arg(args, int);
                }

                convert_number(arg, 10, pad, width, precision, false, true);

                puts(converted);

                format++;
            }
            else if ((*format == 'o') || (*format == 'u') || (*format == 'x') || (*format == 'X'))
            {
                uintmax_t arg = 0;

                if (precision == 3)
                {
                    arg = va_arg(args, unsigned long int);
                }
                else if (precision == 4)
                {
                    arg = va_arg(args, unsigned long long int);
                }
                else
                {
                    arg = va_arg(args, unsigned int);
                }

                if (*format == 'o')
                {
                    if (alternate)
                    {
                        puts("0");
                    }

                    convert_number(arg, 8, pad, width, precision, false, false);
                }
                else if (*format == 'u')
                {
                    convert_number(arg, 10, pad, width, precision, false, false);
                }
                else if (*format == 'x')
                {
                    if (alternate)
                    {
                        puts("0x");
                    }

                    convert_number(arg, 16, pad, width, precision, false, false);
                }
                else
                {
                    if (alternate)
                    {
                        puts("0X");
                    }

                    convert_number(arg, 16, pad, width, precision, true, false);
                }

                puts(converted);

                format++;
            }
            else if (*format == 's')
            {
                const char *arg = va_arg(args, const char *);

                puts(arg);

                format++;
            }
            else if (*format == 'p')
            {
                void *arg = va_arg(args, void *);

                convert_number((uintmax_t)(uintptr_t)arg, 16, pad, width, precision, false, false);

                puts(converted);


                format++;
            }
            else /* if(*format == '%') */
            {
                puts("%");

                if (*format == '%')
                {
                    format++;
                }
            }
        }
        else
        {
            putc(*format);

            format++;
        }
    }

    va_end(args);

    return printed;
}
