#include "../lib/rfcomm.h"
#include <string>

using std::string;

int initRfcommReceive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	int& client,
	int& sock)
{

	bdaddr_t my_bdaddr_any = { { 0, 0, 0, 0, 0, 0 } };
	socklen_t option = sizeof(remote_addr);

	local_address.rc_family = AF_BLUETOOTH;
	local_address.rc_bdaddr = my_bdaddr_any;

	// set to 0, binds the socket to the first available port
	local_address.rc_channel = (uint8_t)0;
	bind(sock, (struct sockaddr *)&local_address, sizeof(local_address));

	// put socket into listening mode
	printf("socket listening mode status: %d\n", listen(sock, 1));

	// accept one connection
	client = accept(sock, (struct sockaddr *)&remote_addr, &option);
	printf("accepted connection\n");

	// client > 0: success
	// client < 0: error
	return client;
}

int rfcomm_receive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	char* buffer,
	int& client,
	string& msg)
{
	int status = 0;
	printf("receiving...\n");

	ba2str(&remote_addr.rc_bdaddr, buffer);
	fprintf(stderr, "accepted connection from %s\n", buffer);
	memset(buffer, 0, strlen(buffer) + 1);

	// read data from the client
	status = read(client, buffer, sizeof(buffer));
	if (status > 0) {
		printf("received \"%s\".\n", buffer);
		msg = buffer;
	}

	// status <= 0: error
	// status >0: return size of received string
	return status;
}
