#ifndef __NFC_DEVICE_H__
#define __NFC_DEVICE_H__

#define ACR122_VENDOR_ID 0x072f
#define ACR122_PRODUCT_ID 0x2200
#define DEVICE_NAME_LENGTH 256

static struct usb_device *nfc_device = NULL;

int nfc_init();

#endif // ! __NFC_DEVICE_H__