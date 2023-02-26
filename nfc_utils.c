#include <stdio.h>
#include <nfc_utils.h>

void print_hex(const char *category, const uint8_t *pbtData, const size_t szBytes)
{
  size_t szPos;

  if( category != NULL ){
    printf("%s: ", category);
  }

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x ", pbtData[szPos]);
  }
  printf("\n");
}

void print_char(const char *category, const uint8_t *buffer, const size_t szBytes)
{
  size_t szPos;

  if( category != NULL ){
    printf("%s: ", category);
  }

  for(szPos = 0; szPos < szBytes; szPos++ ){
    printf("%c", buffer[szPos]);
  }
  printf("\n");
}