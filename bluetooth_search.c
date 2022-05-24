#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bluetooth_search.h"

void open_socket(int *dev_id, int *sock) {
    printf("Open...");
    *dev_id = hci_get_route(NULL);
    *sock = hci_open_dev(*dev_id);
    if (*dev_id < 0 || *sock < 0)
    {
        perror("opening socket");
        exit(1);
    }
    printf("Done\n");
}

inquiry_info* inquiry_bluetooth_device(int len, int max_rsp, int flags, int *num_rsp, int dev_id) {
    printf("Search...");
    inquiry_info *ii = (inquiry_info *)malloc(max_rsp * sizeof(inquiry_info));
    *num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if (*num_rsp < 0)
    {
        perror("hci_inquiry");
    }
    printf("Done\n");
    printf("%d Device%s Detected\n", *num_rsp, *num_rsp > 1 ? "s" : "");
    if (*num_rsp < 1) {
        fprintf(stderr, "Retry\n");
        exit(1);
    }
    return ii;
}

void view_all_device_info(int sock, int num_rsp, inquiry_info **ii, device_info *all_device_info) {
    int i;
    char name[248] = {0};
    for (i = 0; i < num_rsp; i++)
    {
        ba2str(&(*ii + i)->bdaddr, all_device_info[i].addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(*ii + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        strcpy(all_device_info[i].name, name);
        printf("%d: %s\t%s\n", i, all_device_info[i].addr, all_device_info[i].name);
    }
}

int select_device_number(int num_rsp) {
    char select_num[3];
    int i = -1;
    printf("Select Connection Device ...");
    do
    {
        fgets(select_num, sizeof(select_num), stdin);
        sscanf(select_num, "%d", &i);
        if (i < 0)
        {
            printf("Select Number : ");
        }
        else if (i > num_rsp - 1)
        {
            printf("Can't resolve Device Number %d\n", i);
            printf("Select Number : ");
        }
    } while (i < 0 || i > num_rsp - 1);
    return i;
}

device_info select_target()
{
    printf("Bluetooth Client\n");
    printf("Initialize...");
    device_info *all_device_info = {0};
    device_info target_device_info = {0};
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    len = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info *)malloc(max_rsp * sizeof(inquiry_info));
    printf("Done\n");
    open_socket(&dev_id, &sock);
    ii = inquiry_bluetooth_device(len, max_rsp, flags, &num_rsp, dev_id);
    all_device_info = (device_info *)malloc(sizeof(device_info *) * num_rsp);
    view_all_device_info(sock, num_rsp, &ii, all_device_info);
    i = select_device_number(num_rsp);
    strcpy(target_device_info.addr, all_device_info[i].addr);
    strcpy(target_device_info.name, all_device_info[i].name);
    free(ii);
    close(sock);
    return target_device_info;
}
