#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <usb_device.h>


int usb_prepare(void)
{
    int res;

    if(!usb_initialized){
        usb_init();
        usb_initialized = true;
    }

    // usb_find_busses will find all of the busses on the system. Returns the
    // number of changes since previous call to this function (total of new
    // busses and busses removed).
    if( (res = usb_find_busses()) < 0 ){
        printf("Unable to find USB busses (%s)\n", strerror(res));
        return -1;
    }

    // usb_find_devices will find all of the devices on each bus. This should be
    // called after usb_find_busses. Returns the number of changes since the
    // previous call to this function (total of new device and devices removed).    
    if( (res = usb_find_devices()) < 0 ){
        printf("Unable to find USB devices (%s)\n", strerror(res));
        return -1;
    }

    return 0;
}