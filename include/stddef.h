#ifndef STDDEF_H
#define STDDEF_H

// Standard definitions for freestanding environment
typedef unsigned int size_t;
typedef int ptrdiff_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define offsetof(type, member) __builtin_offsetof(type, member)

#endif // STDDEF_H