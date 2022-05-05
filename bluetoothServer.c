#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char *argv[])
{
        printf("Bluetooth Server\n");
        printf("Initialize...");
        struct sockaddr_rc loc_addr = {0}, rem_addr = {0};
        char buf[1024] = {0};
        char client_address[19] = {0};
        int s, client, bytes_read;
        int yes = 1;
        socklen_t opt = sizeof(rem_addr);
        loc_addr.rc_family = AF_BLUETOOTH;
        loc_addr.rc_bdaddr = *BDADDR_ANY;
        loc_addr.rc_channel = (uint8_t) 1;
        printf("Done\n");
        printf("Create Socket...");
        s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (s < 0) {
            perror("Socket");
            return -1;
        }
        printf("Done\n");
        printf("Set Socket...");
        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes)) < 0)
        {
            perror("ERROR on setsockopt");
            exit(1);
        }
        printf("Done\n");
        printf("Bind Socket...");
        if (bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr))) {
            perror("Bind");
            return -1;
        }
        printf("Done\n");
        printf("Listen...");
        if (listen(s, 1)) {
            perror("Listen");
            return -1;
        }
        printf("Done\n");
        while (1) {
            printf("Accept...");
            client = accept(s, (struct sockaddr *)&rem_addr, &opt);
            if (client < 0) {
                perror("Accept");
                exit(1);
            }
            printf("Done\n");
            ba2str(&rem_addr.rc_bdaddr, buf);
            strcpy(client_address, buf);
            fprintf(stderr, "accept connection from %s\n", buf);
            memset(buf, 0, sizeof(buf));
            while (1) {
                bytes_read = read(client, buf, sizeof(buf));
                if (bytes_read > 0 && strlen(buf)) {
                    if (strcmp(buf, "exit") == 0) {
                        close(client);
                        fprintf(stderr, "disconnection from %s\n", client_address);
                        break;
                    }
                    printf("received [%s]\n", buf);
                }
            }
        }
        close(s);
        return 0;
}