#include <stdio.h>
#include <stdlib.h>

#include "bluetooth_system.h"

int main(int argc, char *argv[])
{
    char buf[3];
    int i;
    printf("Select C/S [Server: 1, Client: 2, close: other]: ");
    do {
        fgets(buf, sizeof(buf), stdin);
        i = atoi(buf);
    } while (!i);
    switch (i)
    {
    case 1:
        bluetooth_server();
        break;
    case 2:
        bluetooth_client();
        break;
    default:
        printf("Close\n");
        break;
    }
    return 0;
}