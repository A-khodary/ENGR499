#include "../lib/rfcomm.h"

int rfcomm_receive(int *sock)
{
    struct sockaddr_rc local_address = { 0 }, remote_addr = { 0 };
    char buf[1024] = { 0 };
    int client, bytes_read;
    socklen_t opt = sizeof(remote_addr);

    // allocate socket
	//int sock;
    *sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set to 0, binds the socket to the first available port
    local_address.rc_family = AF_BLUETOOTH;
    local_address.rc_bdaddr = *BDADDR_ANY;
    local_address.rc_channel = (uint8_t) 0;
    bind(*sock, (struct sockaddr *)&local_address, sizeof(local_address));

    // put socket into listening mode
    listen(*sock, 1);

    // accept one connection
    client = accept(*sock, (struct sockaddr *)&remote_addr, &opt);

    ba2str( &remote_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

    // read data from the client
    bytes_read = read(client, buf, sizeof(buf));
    if( bytes_read > 0 ) {
        printf("received [%s]\n", buf);
    }

    // close connection
    close(client);
    close(*sock);
    return 0;
}