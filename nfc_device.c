#include <nfc_device.h>
#include <usb_device.h>

int nfc_init()
{
    int res;

    res = usb_prepare();
    if(res < 0){
        return res;
    }

    res = usb_scan();
    if(res < 0){
        return res;
    }
}