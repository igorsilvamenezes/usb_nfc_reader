#include <stdio.h>
#include <nfc_device.h>

int main(void)
{
    int res;
    nfc_device *pnd;

    printf("App Running...\n");

    pnd = nfc_init();

    if(pnd){
        nfc_open(pnd);

        printf("**LED STATE COMMAND\n");
        nfc_send_led_state(pnd);
    }

    return 0;
}