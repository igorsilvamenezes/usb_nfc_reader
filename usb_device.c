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

int usb_open_device(struct usb_device *pud, struct usb_dev_handle *pudh)
{
    int res;

    //Open the USB device
    pudh = usb_open(pud);
    if(!pudh){
        printf("Unable to open device.\n");
        return 1;
    }

    //Resete the device
    usb_reset(pudh);

    //Claim interface
    res = usb_claim_interface(pudh, 0);
    if(res < 0){
        printf("Ubable to claim USB interface (%s)\n", strerror(res));
        usb_close(pudh);
        return -1;
    }

    //Check if there are more than 0 alternative interfaces and claim the first one
    if(pud->config->interface->altsetting->bAlternateSetting > 0){
        res = usb_set_altinterface(pudh, 0);
        if(res < 0){
            printf("Ubable to set alternate setting on USB interface (%s)\n", strerror(res));
            usb_close(pudh);
            return -1;
        }
    }

    return 0;
}

int usb_get_end_points(struct usb_device *device, usb_desc_data *desc)
{
    uint32_t uiIndex;
    struct usb_endpoint_descriptor pued;
    struct usb_interface_descriptor *puid = device->config->interface->altsetting;

    desc = malloc(sizeof(usb_desc_data));
    if(desc == NULL){
        return -1;
    }

    //There are 3 types of Endpoints: Interrupt In, Bulk In, Bulk Out
    for (uiIndex = 0; uiIndex < puid->bNumEndpoints; uiIndex++) {
        printf("loop {%d}\n", uiIndex);

        //Copy the endpoint to a local var for more readable code
        pued = puid->endpoint[uiIndex];
        
        //Only accept bulk transfer endpoints (ignore interrupt endpoints)
        if(pued.bmAttributes != USB_ENDPOINT_TYPE_BULK)
            continue;

        //Test if is a Bulk In endpoint
        if( (pued.bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_IN ){
            desc->uiEndPointIn = pued.bEndpointAddress;
            desc->uiMaxPacketSize = pued.wMaxPacketSize;

        //Test if is a Bulk Out endpoint
        } else if( (pued.bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_OUT ){
            desc->uiEndPointOut = pued.bEndpointAddress;
            desc->uiMaxPacketSize = pued.wMaxPacketSize;
        }
    }

    return 0;
}