#include <stdio.h>
#include <nfc_utils.h>

void print_hex(const char *category, const uint8_t *pbtData, const size_t szBytes)
{
  size_t szPos;

  printf("%s: ", category);
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x ", pbtData[szPos]);
  }
  printf("\n");
}