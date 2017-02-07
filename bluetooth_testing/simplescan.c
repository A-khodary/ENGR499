#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


//typedef struct {
//	uint8_t b[6];
//} __attribute__((packed)) bdaddr_t;

/*
	The basic data structure used to specify a Bluetooth device address is the
bdaddr_t.All Bluetooth addresses in BlueZ will be stored and manipulated as
bdaddr_t structures.
	BlueZ provides two convenience functions to convert between strings and
bdaddr_t structures.
*/

//int str2ba(const char *str, bdaddr_t *ba);
//int ba2str(const bdaddr_t *ba, char *str);

/*
	str2ba takes an string of the form "XX:XX:XX:XX:XX:XX", where each XX is a
hexadecimal number specifying an octet of the 48-bit address, and packs it into
a 6-byte bdaddr_t. ba2str does exactly the opposite.
*/

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	/*
		Local Bluetooth adapters are assigned identifying numbers starting
	with 0, and a program must specify which adapter to use when allocating
	system resources.
		Usually, there is only one adapter or it doesn't
	matter which one is used, so passing NULL to hci_get_route will retrieve
	the resource number of the first available Bluetooth adapter.
	
	If there are multiple Bluetooth adapters present, then to choose the
	adapter with address "01:23:45 : 67 : 89 : AB", pass the char* of the
	address to hci_devid and use that in place of hci_get_route.
	*/
	//int dev_id = hci_devid("01:23:45:67:89:AB");
	dev_id = hci_get_route(NULL);

/*
		Most Bluetooth operations require the use of an open socket.
	hci_open_dev is a convenience function that opens a Bluetooth socket with
	the specified resource number[2]. To be clear, the socket opened by
	hci_open_dev represents a connection to the microcontroller on the
	specified local Bluetooth adapter, and not a connection to a remote
	Bluetooth device.
		Performing low level Bluetooth operations involves sending commands
	directly to the microcontroller with this socket.
	*/
	sock = hci_open_dev(dev_id);

	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp < 0) perror("hci_inquiry");

	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii + i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
			name, 0) < 0)
			strcpy(name, "[unknown]");
		printf("%s  %s\n", addr, name);
	}

	free(ii);
	close(sock);
	return 0;
}