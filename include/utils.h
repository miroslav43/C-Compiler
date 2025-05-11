#pragma once

#include <stddef.h>
#include <stdnoreturn.h>

noreturn void err(const char *fmt, ...);

void *safeAlloc(size_t nBytes);

char *loadFile(const char *fileName);

// Helper printing functions for tests
void puti(int i);
void puts1(const char *s);
void putc1(char c);
void putd(double d);
