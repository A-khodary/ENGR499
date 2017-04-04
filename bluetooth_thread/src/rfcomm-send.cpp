#include "../lib/rfcomm.h"

void initRfcommSend(int& sock) {
	//
}

int rfcomm_send(int &sock, char* dest, std::string& msg)
{
	struct sockaddr_rc addr = { 0 };
	int status;

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	// set to 0, binds the socket to the first available port
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	
	// send a message
	if (status == 0) {
		status = write(sock, msg.c_str(), msg.size());
	}

	if (status < 0) {
		perror("Error");
		//return 1;
	}

	// status = 0: success
	// status = -1: error
	return status;
}