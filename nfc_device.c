#include <stdio.h>
#include <nfc_device.h>
#include <usb_device.h>
#include <string.h>
#include <nfc_utils.h>

/*Prototypes*/
nfc_device* get_nfc_device(usb_device_list *usb_dev_list);

nfc_device* nfc_init()
{
    int res;
    nfc_device *pnd;
    usb_device_list *device_list;

    pnd = malloc(sizeof(nfc_device));
    if(pnd == NULL ){
        return NULL;
    }

    res = usb_prepare();
    if(res < 0){
        return NULL;
    }

    res = usb_scan(&device_list);
    if(res < 0){
        return NULL;
    }
    
    pnd = get_nfc_device(device_list);
    if(pnd){
        printf("NFC device found at address %d\n", pnd->pud->devnum);
    }else{
        printf("NFC device not found.\n");
        return NULL;
    }

    //usb_close_list(device_list);

    return pnd;
}

int nfc_open(nfc_device *pnd)
{   
    pnd->pudh = usb_open_device(pnd->pud);
    if(pnd->pudh == NULL){
        return -1;
    }

    //Retrice USB device name
    usb_get_device_name(pnd->pud, pnd->pudh, pnd->name, strlen(pnd->name));

    //Retrive the End Points
    usb_get_end_points(pnd->pud, &pnd->pudesc);

    return 0;
}

nfc_device* get_nfc_device(usb_device_list *usb_dev_list)
{
    usb_device_list *current;

    nfc_device *new_device = malloc(sizeof(nfc_device));
    if(!new_device){
        return NULL;
    }
    
    //Iterate through the list of USB devices and return the first nfc device
    for(current = usb_dev_list; current; current = current->next){

        if (current->device->descriptor.idVendor == ACR122_VENDOR_ID &&
            current->device->descriptor.idProduct == ACR122_PRODUCT_ID){

            new_device->pud = current->device;

            break;
        }
    }

    return new_device;
}

int nfc_send_led_state(nfc_device *pnd)
{
    // See ACR122 manual: "Bi-Color LED and Buzzer Control" section
    uint8_t nfc_led_state_frame[] = {
        0x6b, // CCID
        0x09, // lenght of frame
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // padding
        // frame:
        0xff, // Class
        0x00, // INS
        0x40, // P1: Get LED state command
        0x5E, // P2: LED state control (blink Red LED and final Green LED) 0101 1110
        0x04, // Lc
        0x01, 0x02, 0x02, 0x01, // Blinking duration control
    };

    //Write data to device
    print_hex("TX", nfc_led_state_frame, sizeof(nfc_led_state_frame));
    int res = usb_bulk_write(pnd->pudh, pnd->pudesc->uiEndPointOut, (char *)nfc_led_state_frame, sizeof(nfc_led_state_frame), 2000);
    if (res != sizeof(nfc_led_state_frame)) {
        printf("Unable to write to USB (%s)\n", strerror(res));
        return -1;
    }

    unsigned char buffer[ACR122_PACKET_SIZE];
    memset(&buffer, 0x00, sizeof(buffer));

    //Read data from device
    res = usb_bulk_read(pnd->pudh, pnd->pudesc->uiEndPointIn, (char *)buffer, sizeof(buffer), 2000);
    if(res > 0){
        print_hex("RX", buffer, sizeof(buffer));
    } else if (res < 0) {
        printf("Unable to read from USB (%s)\n", strerror(res));
        return -1;
    }

    return 0;
}

int nfc_close(nfc_device *pnd)
{
    usb_close_device(pnd->pudh);
    free(pnd->pudesc);
    free(pnd);
}