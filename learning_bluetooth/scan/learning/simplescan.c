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

//typedef struct {
//	bdaddr_t    bdaddr;
//	uint8_t     pscan_rep_mode;
//	uint8_t     pscan_period_mode;
//	uint8_t     pscan_mode;
//	uint8_t     dev_class[3];
//	uint16_t    clock_offset;
//} __attribute__((packed)) inquiry_info;

/*
	For the most part, only the first entry - the bdaddr field, which gives the
address of the detected device - is of any use.
	Occasionally, there may be a use for the dev_class field, which gives
information about the type of device detected (i.e. if it's a printer, phone,
desktop computer, etc.).
	The rest of the fields are used for low level communication, and are not
useful for most purposes.
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

	/*
		After choosing the local Bluetooth adapter to use and allocating system
	resources, the program is ready to scan for nearby Bluetooth devices.
		In the example, hci_inquiry performs a Bluetooth device discovery and
	returns a list of detected devices and some basic information about them in
	the variable ii.
		On error, it returns - 1 and sets errno accordingly.
	*/

	//int hci_inquiry(int dev_id, int len, int max_rsp, const uint8_t *lap,
	//	inquiry_info **ii, long flags);

	/*
		hci_inquiry is one of the few functions that requires the use of a
	resource number instead of an open socket, so we use the dev_id returned by
	hci_get_route.
		, and , which must be large
	enough to accommodate max_rsp results.
		We suggest using a max_rsp of 255 for a standard 10.24 second inquiry.
	*/

	// At most max_rsp devices will be returned in the output parameter ii
	max_rsp = 255;
	// The inquiry lasts for at most 1.28 * len seconds.
	// (must be large enough to accommodate max_rsp results)
	// (We suggest using a max_rsp of 255 for a standard 10.24 second inquiry.)
	len = 8;
	// IREQ_CACHE_FLUSH: the cache of previously detected devices is flushed
	//		before performing the current inquiry
	// 0: the results of previous inquiries may be returned, even if the
	//		devices aren't in range anymore
	flags = IREQ_CACHE_FLUSH;
	printf("Searching for bluetooth device...\n");
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if (num_rsp < 0) {
		printf("Failed!\n");
		perror("hci_inquiry");
	} else
		printf("Complete!\n");

	/*

		Once a list of nearby Bluetooth devices and their addresses has been
	found, the program determines the user-friendly names associated with those
	addresses and presents them to the user.
	*/
	/*
		On success, hci_read_remote_name returns 0 and store the device's name.
		On failure, it returns -1 and sets errno accordingly.

		timeout: (in milliseconds) time used to query the user-friendly name
		ba: address of Bluetooth device
		len: max number of bytes of the device's name
		name: the first len bytes of device name
	*/
	//int hci_read_remote_name(int sock, const bdaddr_t *ba, int len,
	//	char *name, int timeout)
	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii + i)->bdaddr, addr);
		memset(name, 0, sizeof(name));

		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
			name, 0) < 0)
			strcpy(name, "[unknown]");

		printf("%s  %s\n", addr, name);
	}

	free(ii);
	close(sock); // a system call that is used to close an open file descriptor
	return 0;
}