#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	dev_id = hci_get_route(NULL); // get bluetooth device from host
	sock = hci_open_dev(dev_id); // create a socket for communication

	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}

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