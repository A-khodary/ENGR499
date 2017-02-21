#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

struct sockaddr_rc {
	sa_family_t	rc_family; // should always be AF_BLUETOOTH.
	bdaddr_t	rc_bdaddr; // For a listening socket, specifies the local
						   // Bluetooth adapter to use, and is typically set
						   // to BDADDR_ANY (any local Bluetooth adapter)
	uint8_t		rc_channel; // For listening sockets, specifies the port number
							// to listen on
};

// Bluetooth byte ordering is little-endian (the least significant bytes
// are transmitted first)
// They are used when filling in the socket addressing structures,// communicating with the Bluetooth microcontroller, and when performing// low level operations on transport protocol sockets// 16 bit unsigned int to Bluetooth byte orderunsigned short int htobs(unsigned short int num);
// convert back
unsigned short int btohs(unsigned short int num);

// 32 bit unsigned integers to Bluetooth byte order
unsigned int htobl(unsigned int num);
// convert back
unsigned int btohl(unsigned int num);

// For Linux kernel versions ealier than 2.6.7
// when bind doesn't fail, return 0
int dynamic_bind_rc(int sock, struct sockaddr_rc *sockaddr, uint8_t *port)
{
	// The process for L2CAP sockets is similar, but tries odd numbered ports
	// 4097 - 32767 (0x1001 - 0x7FFF) instead of ports 1 - 30
	int err;
	for (*port = 1; *port <= 31; *port++) {
		sockaddr->rc_channel = *port;
		err = bind(sock, (struct sockaddr *)sockaddr, sizeof(sockaddr));
		if (!err || errno == EINVAL)
			break;
	}

	if (port == 31) {
		err = -1;
		errno = EINVAL;
	}

	return err;
}

int main(int argc, char **argv)
{
	// To establish an RFCOMM connection with another Bluetooth device
	// create and fill out a struct sockaddr_rc addressing structure
	struct sockaddr_rc addr = { 0 };
	int s, status;
	char dest[18] = "01:23:45:67:89:AB";

	// allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	if (status == 0) {
		status = write(s, "hello!", 6);
	}

	if (status < 0) perror("uh oh");

	close(s);
	return 0;
}