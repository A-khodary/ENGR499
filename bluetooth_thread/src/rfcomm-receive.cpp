#include "../lib/rfcomm.h"

void initRfcommReceive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	bdaddr_t my_bdaddr_any,
	socklen_t opt,
	int& client,
	int& sock)
{

	local_address.rc_family = AF_BLUETOOTH;
	local_address.rc_bdaddr = my_bdaddr_any;

	// set to 0, binds the socket to the first available port
	local_address.rc_channel = (uint8_t)0;
	bind(sock, (struct sockaddr *)&local_address, sizeof(local_address));

	// put socket into listening mode
	printf("socket listening mode status: %d\n", listen(sock, 1));

	// accept one connection
	client = accept(sock, (struct sockaddr *)&remote_addr, &opt);
	printf("accepted connection\n");
}

int rfcomm_receive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	bdaddr_t my_bdaddr_any,
	char* buffer,
	socklen_t opt,
	int& client,
	int& sock)
{
	int status = 0;
	printf("receiving...\n");


	ba2str(&remote_addr.rc_bdaddr, buffer);
	memset(buffer, 0, strlen(buffer) + 1);

	// read data from the client
	status = read(client, buffer, sizeof(buffer));
	if (status > 0) {
		printf("received \"%s\".\n", buffer);
	}

	// status <= 0: error
	// status >0: return size of received string
	return status;
}
