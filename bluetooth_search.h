#ifndef BLUETOOTH_SEARCH_H
#define BLUETOOTH_SEARCH_H

#include "bluetooth_type.h"

void open_socket(int *dev_id, int *sock);

inquiry_info* inquiry_bluetooth_device(int len, int max_rsp, int flags, int *num_rsp, int dev_id);

void view_all_device_info(int sock, int num_rsp, inquiry_info **ii, device_info *all_device_info);

int select_device_number(int num_rsp);

device_info select_target();

#endif