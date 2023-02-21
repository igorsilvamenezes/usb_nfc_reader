#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include <usb.h>
#include <stdbool.h>

#define DEVICE_NAME_LENGTH 256

typedef struct usb_device_list {
    struct usb_device *device;
    struct usb_device_list *next;
} usb_device_list;

typedef struct {
    uint32_t uiEndPointIn;
    uint32_t uiEndPointOut;
    uint32_t uiMaxPacketSize;
} usb_desc_data;

static bool usb_initialized;
static bool usb_opened;

int usb_prepare(void);
int usb_scan(usb_device_list **device_list);
usb_dev_handle *usb_open_device(struct usb_device *pud);
bool usb_get_device_name(struct usb_device *device, usb_dev_handle *udev, char *buffer, size_t len);
int usb_get_end_points(struct usb_device *device, usb_desc_data **pudd);
int usb_close_device(struct usb_dev_handle *pudh);
void usb_close_list(usb_device_list *device_list);

#endif // ! __USB_DEVICE_H__