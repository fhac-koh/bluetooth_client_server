#ifndef BLUETOOTH_CONNECT_H
#define BLUETOOTH_CONNECT_H

#include "bluetooth_type.h"

struct sockaddr_rc bluetooth_socket_initialize();

int bluetooth_socket_create();

void bluetooth_socket_set(int s, int optval);

void bluetooth_socket_bind(int s, struct sockaddr_rc loc_addr);

void bluetooth_socket_listen(int s);

connection_info bluetooth_socket_connect(int s, struct sockaddr_rc addr);

connection_info connect_target(device_info target);

void bluetooth_socket_accept(int s, int *client, struct sockaddr_rc rem_addr, socklen_t opt, char *client_address);

int bluetooth_socket_send(connection_info target);

int bluetooth_socket_recieve(int client, char *buf, char *client_address);

int recieve_fin_message(int client, char *buf);

#endif