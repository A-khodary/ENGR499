#include "lib/config.h"
#include "lib/scan.h"

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	int max_rsp = 255, num_rsp = -1;
	int dev_id, sock;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	if (bt_scan(&ii, max_rsp, &num_rsp, &dev_id, &sock)) {
		exit(1);
	}

	int i;
	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii + i)->bdaddr, addr);
		memset(name, 0, sizeof(name));

		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
			name, 0) < 0) {
			printf("%s  [unknown]\n", addr);
		}
		else {
			printf("%s  %s\n", addr, name);
		}
	}

	free(ii);
	close(sock); // a system call that is used to close an open file descriptor
	return 0;
}