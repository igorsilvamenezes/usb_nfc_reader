#ifndef __NFC_DEVICE_H__
#define __NFC_DEVICE_H__

#include <usb.h>
#include <usb_device.h>

#define ACR122_VENDOR_ID 0x072f
#define ACR122_PRODUCT_ID 0x2200
#define DEVICE_NAME_LENGTH 256

#define ACR122_PACKET_SIZE 64

typedef struct {
    char name[DEVICE_NAME_LENGTH];
    struct usb_device *pud;
    usb_dev_handle *pudh;
    usb_descriptor *pudesc;
} nfc_device;

nfc_device* nfc_init();
int nfc_open(nfc_device *pnd);
int nfc_close(nfc_device *pnd);
int nfc_send_led_state(nfc_device *nfc_dev);
int nfc_send_get_firmware_version(nfc_device *pnd);
int nfc_send_led_state_apdu(nfc_device *nfc_dev);

#endif // ! __NFC_DEVICE_H__