#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

typedef struct
{
        char addr[19];
        char name[248];
} device_info;

typedef struct {
        int socket;
        int status;
} connection_info;

device_info select_target()
{
        device_info target_device_info = {0};
        inquiry_info *ii = NULL;
        int max_rsp, num_rsp;
        int dev_id, sock, len, flags;
        int i;
        char select_num[3];
        char addr[19] = {0};
        char name[248] = {0};
        char **all_addr;
        char **all_name;

        dev_id = hci_get_route(NULL);
        sock = hci_open_dev(dev_id);
        if (dev_id < 0 || sock < 0)
        {
                perror("opening socket");
                exit(1);
        }

        len = 8;
        max_rsp = 255;
        flags = IREQ_CACHE_FLUSH;
        ii = (inquiry_info *)malloc(max_rsp * sizeof(inquiry_info));
        num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
        if (num_rsp < 0)
        {
                perror("hci_inquiry");
        }
        printf("%d Device%s Detected\n", num_rsp, num_rsp > 1 ? "s" : "");
        all_addr = (char **)malloc(sizeof(char *) * num_rsp);
        all_name = (char **)malloc(sizeof(char *) * num_rsp);
        for (i = 0; i < num_rsp; i++)
        {
                all_addr[i] = (char *)malloc(sizeof(char) * 19);
                all_name[i] = (char *)malloc(sizeof(char) * 248);
        }
        for (i = 0; i < num_rsp; i++)
        {
                ba2str(&(ii + i)->bdaddr, addr);
                memset(name, 0, sizeof(name));
                if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0)
                {
                        strcpy(name, "[unknown]");
                }
                strcpy(all_addr[i], addr);
                strcpy(all_name[i], name);
                printf("%d: %s\t%s\n", i, all_addr[i], all_name[i]);
        }
        if (*all_addr == NULL)
        {
                printf("No detected\n");
                exit(1);
        }
        i = -1;
        printf("Select Connection Device ...");
        do
        {
                fgets(select_num, sizeof(select_num), stdin);
                sscanf(select_num, "%d", &i);
                if (i < 0)
                {
                        printf("Not a Number\n");
                        printf("Select Number : ");
                }
                else if (i > num_rsp - 1)
                {
                        printf("Can't resolve Device Number %d\n", i);
                        printf("Select Number : ");
                }
        } while (i < 0 || i > num_rsp - 1);
        strcpy(target_device_info.addr, all_addr[i]);
        strcpy(target_device_info.name, all_name[i]);
        free(ii);
        for (i = 0; i < num_rsp; i++)
        {
                free(all_addr[i]);
                free(all_name[i]);
        }
        close(sock);
        return target_device_info;
}

connection_info connect_target(device_info target) {
        connection_info target_connection_info = {0};
        struct sockaddr_rc addr = {0};
        int s, status;
        s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = (uint8_t) 1;
        str2ba(target.addr, &addr.rc_bdaddr);
        status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        target_connection_info.socket = s;
        target_connection_info.status = status;
        return target_connection_info;
}

int main(int argc, char *argv[])
{
        device_info target = select_target();
        printf("Target address: %s, name: %s\n", target.addr, target.name);
        connection_info target_connection_info = connect_target(target);
        char buf[1024] = {0};
        printf("Enter 'exit' to disconnect\n");
        while (target_connection_info.status >= 0) {
                printf("send message > ");
                fgets(buf, sizeof(buf), stdin);
                buf[strlen(buf) - 1] = '\0';
                target_connection_info.status = write(target_connection_info.socket, buf, sizeof(buf));
                if (target_connection_info.status < 0) {
                        perror("connection_fail");
                        break;
                }
                if (strcmp(buf, "exit") == 0) {
                        printf("disconnect\n");
                        break;
                }
        }
        close(target_connection_info.socket);
        return 0;
}