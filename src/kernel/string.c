/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 悪意
 *  Copyright (C) 2010 - 2011 Patrick Pokatilo
 *
 *  This file is part of Nuke (悪意).
 *
 *  Nuke (悪意) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nuke (悪意) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nuke (悪意).  If not, see <http://www.gnu.org/licenses/>.
 */

#include "string.h"

void *memset(void *s, int c, size_t n)
{
    char *xs = s;

    while (n--)
    {
        *(xs++) = c;
    }

    return s;
}

void *memcpy(void *dest, void *src, size_t n)
{
    if (dest == src)
    {
        return dest;
    }

    char *xd = dest;
    const char *xs = src;

    while (n--)
    {
        *(xd++) = *(xs++);
    }

    return dest;
}

void *memmove(void *dest, void *src, size_t n)
{
    if (dest == src)
    {
        return dest;
    }

    char *xd;
    const char *xs;

    if (dest <= src)
    {
        xd = dest;
        xs = src;

        while (n--)
        {
            *(xd++) = *(xs++);
        }
    }
    else
    {
        xd = dest;
        xd += n;
        xs = src;
        xs += n;

        while (n--)
        {
            *(--xd) = *(--xs);
        }
    }

    return dest;
}
