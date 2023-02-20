#include <stdio.h>
#include <nfc_device.h>

int main(void)
{
    int res;

    printf("App Running...\n");

    nfc_init();

    nfc_open();

    return 0;
}