#include <stdio.h>
#include <nfc_device.h>

int main(void)
{
    int res;

    printf("App Running...\n");

    nfc_init();

    return 0;
}