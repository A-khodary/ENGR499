#include "../lib/rfcomm.h"

int rfcomm_send(char* dest)
{
	struct sockaddr_rc addr = { 0 };
	int sock, status;

	// allocate a socket
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	if (status == 0) {
		status = write(sock, "If you get this, it means the program worked. Please email me, so I can proceed", 6);
	}

	if (status < 0)
		return 1;

	close(sock);
	return 0;
}