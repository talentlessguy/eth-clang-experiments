#ifndef NAMEHASH_H
#define NAMEHASH_H

#include <stdint.h>
#include <stdlib.h>
#include <ethc/keccak256.h>
#include <ethc/hex.h>
#include <string.h>
#include <stdio.h>

char *labelhash(const char *label);
char *namehash(const char *name);

#endif /* NAMEHASH_H */
