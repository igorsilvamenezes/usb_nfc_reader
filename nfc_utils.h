#ifndef __NFC_UTILS_H__
#define __NFC_UTILS_H__

#include <stdint.h>
#include <stddef.h>

void print_hex(const char *category, const uint8_t *pbtData, const size_t szBytes);
void print_char(const char *category, const uint8_t *buffer, const size_t szBytes);

#endif // ! __NFC_UTILS_H__
