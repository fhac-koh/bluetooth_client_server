#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bluetooth_search.h"
#include "bluetooth_connect.h"

#include "bluetooth_system.h"

void bluetooth_client() {
    device_info device = select_target();
    printf("Target address: %s, name: %s\n", device.addr, device.name);
    connection_info target = connect_target(device);
    int continue_flag = 1;
    printf("Enter 'exit' to disconnect\n");
    while (target.status >= 0 && continue_flag)
    {
        continue_flag = bluetooth_socket_send(target);
    }
    close(target.socket);
}

void bluetooth_server() {
    printf("Bluetooth Server\n");
    printf("Initialize...");
    struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
    char buf[1024] = {0};
    char client_address[19] = {0};
    int s, client, bytes_read;
    int yes = 1;
    socklen_t opt = sizeof(rem_addr);
    loc_addr = bluetooth_socket_initialize();
    printf("Done\n");
    s = bluetooth_socket_create();
    bluetooth_socket_set(s, yes);
    bluetooth_socket_bind(s, loc_addr);
    bluetooth_socket_listen(s);
    while (1)
    {
        bluetooth_socket_accept(s, &client, rem_addr, opt, client_address);
        while (1)
        {
            bytes_read = bluetooth_socket_recieve(client, buf, client_address);
            if (recieve_fin_message(client, buf)) {
                fprintf(stderr, "disconnection from %s\n", client_address);
                break;
            }
        }
    }
    close(s);
}
