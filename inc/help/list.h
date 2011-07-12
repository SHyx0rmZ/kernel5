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

#ifndef _HELP_LIST_H_
#define _HELP_LIST_H_

#include "memory.h"

#define HELP_DLLIST(t, l) \
    struct help_dllist_node_##t \
    { \
        t *data; \
        struct help_dllist_node_##t *prev; \
        struct help_dllist_node_##t *next; \
    }; \
    \
    struct help_dllist_##t \
    { \
        struct help_dllist_node_##t *start; \
        struct help_dllist_node_##t *end; \
    } l;

#define HELP_DLNODE(l) \
    __typeof__(l.start)

#define HELP_DLLIST_ADD(l, n) \
    { \
        if (l.end != NULL) \
        { \
            l.end->next = n; \
            n->prev = l.end; \
            l.end = n; \
            n->next = NULL; \
        } \
        else \
        { \
            l.end = n; \
            l.start = n; \
            n->prev = NULL; \
            n->next = NULL; \
        }\
    }

#define HELP_DLLIST_FIND(l, d, n) \
    __typeof__(l.start) n = NULL; \
    \
    { \
        if (l.start != NULL) \
        { \
            __typeof__(l.start) t = l.start; \
            \
            while ((t->data != d) && (t != NULL)) \
            { \
                t = t->next; \
            } \
            \
            n = t; \
        } \
    }

#define HELP_DLLIST_REMOVE(l, n) \
    { \
        if (l.start != NULL) \
        { \
            __typeof__(l.start) t = l.start; \
            \
            while ((t != n) && (t != NULL)) \
            { \
                t = t->next; \
            } \
            \
            if (t != NULL) \
            { \
                if (t->prev != NULL) \
                { \
                    t->prev->next = t->next; \
                } \
                \
                if (t->next != NULL) \
                { \
                   t->next->prev = t->prev; \
                } \
                \
                if (t == l.end) \
                { \
                    l.end = t->prev; \
                } \
                \
                if (t == l.start) \
                { \
                    l.start = NULL; \
                } \
            } \
        } \
    }

#endif
