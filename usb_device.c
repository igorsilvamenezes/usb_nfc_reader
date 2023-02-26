#include <stdio.h>
#include <string.h>
#include <usb_device.h>
#include <nfc_utils.h>

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

int usb_scan(usb_device_list **device_list)
{
    struct usb_bus *bus;
    struct usb_device *device;
    usb_dev_handle *handle;
    bool usb_close_end;

    char device_name [DEVICE_NAME_LENGTH];

    for(bus = usb_get_busses(); bus; bus = bus->next ){
        for(device = bus->devices; device; device = device->next){

            //printf("Device class: %d\n", device->descriptor.bDeviceClass );
            if(device->descriptor.bDeviceClass != USB_CLASS_PER_INTERFACE){
                continue;
            }

            //add the device to de list
            usb_device_list *new_device = malloc(sizeof(usb_device_list));
            new_device->device = device;
            new_device->next = *device_list;
            *device_list = new_device;

            printf("USB device found at ");
            printf("Bus %s ", bus->dirname);
            printf("Address %02d ", device->devnum);
            printf("Device %s ", device->filename);
            printf("ID %04x:%04x ", device->descriptor.idVendor, device->descriptor.idProduct);

            if(!usb_opened){
                usb_close_end = true;
                handle = usb_open(device);
            }

            if(handle){
                usb_get_device_name(device, handle, device_name, strlen(device_name));
                printf("%s", device_name);
            }

            printf("\n");

            if(usb_close_end){
                usb_close(handle);
            }
        }
    }

    return 0;
}

bool usb_get_device_name(struct usb_device *device, usb_dev_handle *udev, char *buffer, size_t len)
{
    *buffer = '\0';

    if(device->descriptor.iManufacturer || device->descriptor.idProduct){
        if(udev){
            usb_get_string_simple(udev, device->descriptor.iManufacturer, buffer, len);
            if(strlen(buffer) > 0){
                strcpy(buffer + strlen(buffer), " / ");
            }

            usb_get_string_simple(udev, device->descriptor.iProduct, buffer + strlen(buffer), len - strlen(buffer));

            return true;
        }
    }

    return false;
}

usb_dev_handle *usb_open_device(struct usb_device *pud)
{
    int res;
    usb_dev_handle *handle;

    //Open the USB device
    handle = usb_open(pud);
    if(!handle){
        printf("Unable to open device.\n");
        return NULL;
    }

    //Reset the device
    usb_reset(handle);

    //Claim interface
    res = usb_claim_interface(handle, 0);
    if(res < 0){
        printf("Ubable to claim USB interface (%s)\n", strerror(res));
        usb_close(handle);
        return NULL;
    }

    //Check if there are more than 0 alternative interfaces and claim the first one
    if(pud->config->interface->altsetting->bAlternateSetting > 0){
        res = usb_set_altinterface(handle, 0);
        if(res < 0){
            printf("Unable to set alternate setting on USB interface (%s)\n", strerror(res));
            usb_close(handle);
            return NULL;
        }
    }

    return handle;
}

int usb_get_end_points(struct usb_device *device, usb_descriptor **pudesc)
{
    uint32_t uiIndex;
    usb_descriptor *descriptor;
    struct usb_endpoint_descriptor pued;
    struct usb_interface_descriptor *puid = device->config->interface->altsetting;

    descriptor = malloc(sizeof(usb_descriptor));
    if(descriptor == NULL){
        return -1;
    }

    //There are 3 types of Endpoints: Interrupt In, Bulk In, Bulk Out
    for (uiIndex = 0; uiIndex < puid->bNumEndpoints; uiIndex++) {

        //Copy the endpoint to a local var for more readable code
        pued = puid->endpoint[uiIndex];
        
        //Only accept bulk transfer endpoints (ignore interrupt endpoints)
        if(pued.bmAttributes != USB_ENDPOINT_TYPE_BULK)
            continue;

        //Test if is a Bulk In endpoint
        if( (pued.bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_IN ){
            descriptor->uiEndPointIn = pued.bEndpointAddress;
            descriptor->uiMaxPacketSize = pued.wMaxPacketSize;

        //Test if is a Bulk Out endpoint
        } else if( (pued.bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_OUT ){
            descriptor->uiEndPointOut = pued.bEndpointAddress;
            descriptor->uiMaxPacketSize = pued.wMaxPacketSize;
        }
    }

    *pudesc = descriptor;

    return 0;
}

int usb_close_device(struct usb_dev_handle *pudh)
{
    int res;

    if( (res = usb_release_interface(pudh, 0)) < 0){
        printf("Unable to release USB interface (%s)\n", strerror(res));
        return res;
    }

    if( (res = usb_close(pudh)) < 0 ){
        printf("unable to close USB connection (%s)\n", strerror(res));
        return res;
    }

    return 0;
}

void usb_close_list(usb_device_list *device_list)
{
    //free the list when we're done with it
    while(device_list){
        usb_device_list *temp = device_list;
        device_list = device_list->next;
        free(temp);
    }

    free(device_list);
}

int usb_send_apdu(usb_device_data *device_data, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t le, const uint8_t *data, const size_t data_len)
{
    nfc_usb_apdu_frame frame;
    memset(&frame, 0x00, sizeof(frame));

    frame.ccid_header.bMessageType = PC_to_RDR_Escape;
    frame.ccid_header.dwLength = htole32(data_len + sizeof(nfc_apdu_header));

    frame.apdu_header.bClass = NFC_COMMAND_CLASS;
    frame.apdu_header.bIns = ins;
    frame.apdu_header.bP1 = p1;
    frame.apdu_header.bP2 = p2;
    frame.apdu_header.bLen = le;

    if( (data != NULL) && (data_len > 0) ){
        memcpy(frame.apdu_payload, data, data_len);
    }

    int frame_len = (sizeof(usb_ccid_header) + sizeof(nfc_apdu_header) + data_len);

    //Write data to device
    print_hex("TX", (unsigned char *)&frame, frame_len);
    int res = usb_bulk_write(device_data->pudh, device_data->pudesc->uiEndPointOut, (unsigned char *)&frame, frame_len, 2000);
    if (res != frame_len) {
        printf("Unable to write to USB (%s)\n", strerror(res));
        return -1;
    }
    
    unsigned char buffer[ACR122_PACKET_SIZE];
    memset(&buffer, 0x00, sizeof(buffer));

    //Read data from device
    res = usb_bulk_read(device_data->pudh, device_data->pudesc->uiEndPointIn, (char *)buffer, sizeof(buffer), 2000);
    if(res > 0){
        print_hex("RX", buffer, sizeof(buffer));
    } else if (res < 0) {
        printf("Unable to read from USB (%s)\n", strerror(res));
        return -1;
    }

    off_t offset = 0;
    usb_ccid_response response_data;

    response_data.bMessageType = buffer[offset++];
    response_data.dwLength = (buffer[offset++]) | (buffer[offset++] << 8) | (buffer[offset++] << 16) | (buffer[offset++] << 24);
    response_data.bSlot = buffer[offset++];
    response_data.bSeq = buffer[offset++];
    response_data.bStatus = buffer[offset++];
    response_data.bError = buffer[offset++];
    response_data.bRFU = buffer[offset++];

    response_data.data = malloc(response_data.dwLength);
    memcpy(response_data.data, &buffer[offset], response_data.dwLength);

    print_hex("response_data", (unsigned char *)&response_data, sizeof(usb_ccid_response) - 8);
    print_hex("response_data.data", response_data.data, response_data.dwLength);

    return 0;
}