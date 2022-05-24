#ifndef BLUETOOTH_TYPE_H
#define BLUETOOTH_TYPE_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

typedef struct
{
    char addr[19];
    char name[248];
} device_info;

typedef struct
{
    int socket;
    int status;
} connection_info;

#endif