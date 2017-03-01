#include "../lib/rfcomm.h"

int rfcomm_send(int *sock, char* dest)
{
	struct sockaddr_rc addr = { 0 };
	int status;

	// allocate a socket
	//int sock;
	//sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	// set to 0, binds the socket to the first available port
	addr.rc_channel = (uint8_t) 1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(*sock, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	if (status == 0) {
		status = write(*sock, "If you get this, it means the program worked. Please email me, so I can proceed", 6);
	}

	//close(sock);
	if (status < 0) {
		perror("Error");
		return 1;
	}
	return 0;
}