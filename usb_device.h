#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include <usb.h>
#include <stdbool.h>

#define DEVICE_NAME_LENGTH 256

typedef struct usb_device_list {
    struct usb_device *device;
    struct usb_device_list *next;
} usb_device_list;

static bool usb_initialized;
static bool usb_opened;

int usb_prepare(void);
int usb_scan(usb_device_list **device_list);

#endif // ! __USB_DEVICE_H__