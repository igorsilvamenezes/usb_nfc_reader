#ifndef __NFC_DEVICE_H__
#define __NFC_DEVICE_H__

#include <usb.h>
#include <usb_device.h>

#define ACR122_VENDOR_ID 0x072f
#define ACR122_PRODUCT_ID 0x2200
#define DEVICE_NAME_LENGTH 256

typedef struct {
    char name[DEVICE_NAME_LENGTH];
    struct usb_device *pud;
    usb_dev_handle *pudh;
    usb_desc_data *desc_data;
} nfc_device;

static nfc_device *pnd = NULL;

int nfc_init();
int nfc_open();

#endif // ! __NFC_DEVICE_H__