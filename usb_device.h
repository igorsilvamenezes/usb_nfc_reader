#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include <usb.h>
#include <stdbool.h>

#define DEVICE_NAME_LENGTH 256

#define ACR122_PACKET_SIZE 64

// CCID Bulk-Out messages type
#define PC_to_RDR_IccPowerOn    0x62
#define PC_to_RDR_XfrBlock      0x6f
#define PC_to_RDR_Escape        0x6b

#define NFC_COMMAND_CLASS       0XFF

typedef struct usb_device_list {
    struct usb_device *device;
    struct usb_device_list *next;
} usb_device_list;

typedef struct {
    uint32_t uiEndPointIn;
    uint32_t uiEndPointOut;
    uint32_t uiMaxPacketSize;
} usb_descriptor;

typedef struct {
    struct usb_device *device;
    usb_dev_handle *pudh;
    usb_descriptor *pudesc;
} usb_device_data;

/* The #pragma pack(1) directive instructs the compiler to pack the structure 
members together without any padding. This means that the compiler will not 
insert any additional bytes between the structure members to align them with 
a boundary. By default, the compiler aligns the structure members on word 
boundaries (or multiples of 4 bytes).
This is useful in situations where the structure needs to be packed tightly 
in memory to ensure that its size is minimized and that it can be transmitted 
over a network or stored in a file in a compact form. */
#pragma pack(1)

 /* Header to CCID (Intergrated Circuit Cards Interface Device) */
typedef struct {
  uint8_t bMessageType;
  uint32_t dwLength;
  uint8_t bSlot;
  uint8_t bSeq;
  uint8_t bMessageSpecific[3];
} usb_ccid_header;

/* Header to APDU (Application Protocol Data Unit) */
typedef struct {
  uint8_t bClass;
  uint8_t bIns;
  uint8_t bP1;
  uint8_t bP2;
  uint8_t bLen;
} nfc_apdu_header;

/* Structure that contains the data to be sent to an APDU command */
typedef struct {
  usb_ccid_header ccid_header;
  nfc_apdu_header apdu_header;
  uint8_t apdu_payload[255];
} nfc_usb_apdu_frame;

#pragma pack()

static bool usb_initialized;
static bool usb_opened;

int usb_prepare(void);
int usb_scan(usb_device_list **device_list);
usb_dev_handle *usb_open_device(struct usb_device *pud);
bool usb_get_device_name(struct usb_device *device, usb_dev_handle *udev, char *buffer, size_t len);
int usb_get_end_points(struct usb_device *device, usb_descriptor **pudd);
int usb_close_device(struct usb_dev_handle *pudh);
void usb_close_list(usb_device_list *device_list);
int usb_send_apdu(usb_device_data *device_data, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t le, const uint8_t *data, const size_t data_len);

#endif // ! __USB_DEVICE_H__