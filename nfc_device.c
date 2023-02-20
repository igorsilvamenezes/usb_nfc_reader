#include <stdio.h>
#include <nfc_device.h>
#include <usb_device.h>
#include <string.h>

/*Prototypes*/
nfc_device* get_nfc_device(usb_device_list *usb_dev_list);

int nfc_init()
{
    int res;
    usb_device_list *device_list = NULL;    

    res = usb_prepare();
    if(res < 0){
        return res;
    }

    res = usb_scan(&device_list);
    if(res < 0){
        return res;
    }
    
    pnd = get_nfc_device(device_list);
    if(pnd){
        printf("NFC device found at address %d\n", pnd->pud->devnum);
    }else{
        printf("NFC device not found.\n");
        return 1;
    }
}

int nfc_open()
{
    int res;
    
    res = usb_open_device(pnd->pud, pnd->pudh);
    if(res < 0){
        return res;
    }

    //Retrice USB device name
    usb_get_device_name(pnd->pud, pnd->pudh, pnd->name, strlen(pnd->name));

    //Retrive the End Points
    usb_get_end_points(pnd->pud, pnd->desc_data);

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