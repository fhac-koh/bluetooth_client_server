#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "bluetooth_connect.h"

struct sockaddr_rc bluetooth_socket_initialize()
{
    struct sockaddr_rc loc_addr;
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t)1;
    return loc_addr;
}

int bluetooth_socket_create()
{
    int s;
    printf("Create Socket...");
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (s < 0)
    {
        perror("Socket");
        exit(1);
    }
    printf("Done\n");
    return s;
}

void bluetooth_socket_set(int s, int optval)
{
    printf("Set Socket...");
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
    {
        perror("ERROR on setsockopt");
        exit(1);
    }
    printf("Done\n");
}

void bluetooth_socket_bind(int s, struct sockaddr_rc loc_addr)
{
    printf("Bind Socket...");
    if (bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)))
    {
        perror("Bind");
        exit(1);
    }
    printf("Done\n");
}

void bluetooth_socket_listen(int s)
{
    printf("Listen...");
    if (listen(s, 1))
    {
        perror("Listen");
        exit(1);
    }
    printf("Done\n");
}

connection_info bluetooth_socket_connect(int s, struct sockaddr_rc addr)
{
    printf("Connect...");
    connection_info target_connection_info = {0};
    target_connection_info.socket = s;
    target_connection_info.status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (target_connection_info.status < 0)
    {
        perror("Connect");
        exit(1);
    }
    printf("Done\n");
    return target_connection_info;
}

void bluetooth_socket_accept(int s, int *client, struct sockaddr_rc rem_addr, socklen_t opt, char *client_address)
{
    printf("Accept...");
    *client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    if (*client < 0)
    {
        perror("Accept");
        exit(1);
    }
    ba2str(&rem_addr.rc_bdaddr, client_address);
    fprintf(stderr, "accept connection from %s\n", client_address);
    printf("Done\n");
}

int bluetooth_socket_send(connection_info target) {
    char buf[1024] = {0};
    printf("send message > ");
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf) - 1] = '\0';
    target.status = write(target.socket, buf, sizeof(buf));
    if (target.status < 0)
    {
        perror("connection_fail");
        return 0;
    }
    if (strcmp(buf, "exit") == 0)
    {
        printf("disconnect\n");
        return 0;
    }
    return 1;
}

int bluetooth_socket_recieve(int client, char *buf, char *client_address)
{
    int bytes_read;
    memset(buf, 0, strlen(buf));
    bytes_read = read(client, buf, sizeof(char) * 1024);
    if (bytes_read > 0 && strlen(buf))
    {
        printf("received [%s]\n", buf);
    }
}

int recieve_fin_message(int client, char *buf) {
    if (strcmp(buf, "exit") == 0)
    {
        close(client);
        return 1;
    }
    return 0;
}

connection_info connect_target(device_info target)
{
    connection_info target_connection_info = {0};
    struct sockaddr_rc addr = {0};
    int s, status;
    addr = bluetooth_socket_initialize();
    s = bluetooth_socket_create();
    str2ba(target.addr, &addr.rc_bdaddr);
    target_connection_info = bluetooth_socket_connect(s, addr);
    return target_connection_info;
}
