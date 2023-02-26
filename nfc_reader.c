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

        printf("**GET FIRMWARE VERSION COMMAND\n");
        nfc_send_get_firmware_version(pnd);

        nfc_close(pnd);
    }

    return 0;
}