#include "../lib/rfcomm.h"

int initRfcommSend(int& sock, char* dest) {
	//
	struct sockaddr_rc addr = { 0 };
	int status;

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	// set to 0, binds the socket to the first available port
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));

	if (status < 0) {
		printf("Could not connect %s for sending!!\n", dest);
		//return 1;
	}

	return status;
}

int rfcomm_send(int &sock, char* dest, std::string& msg)
{	
	int status = 0;
	(void) dest;
	status = write(sock, msg.c_str(), msg.size());
	

	if (status < 0) {
		perror("Error");
		//return 1;
	}

	// status = 0: success
	// status = -1: error
	return status;
}
