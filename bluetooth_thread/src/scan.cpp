#include "../lib/scan.h"

int bt_scan(inquiry_info **ii, int max_rsp, int &num_rsp, int &sock) {
	int dev_id;

	// calculate timeout time
	int len = (int) ((double) max_rsp / 255.0 * 8.0);

	// call helper function
	return bt_scan_helper(ii, max_rsp, num_rsp, dev_id,
		sock, len, IREQ_CACHE_FLUSH);
}

int bt_scan_helper(inquiry_info **ii, int max_rsp, int &num_rsp, int &dev_id,
	int &sock, int len, int flags)
{
	dev_id = hci_get_route(NULL);

	sock = hci_open_dev(dev_id);

	if (dev_id < 0 || sock < 0) {
		perror("Opening socket");
		return 1;
	}

	printf("Searching for bluetooth device...\n");
	//*ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));
	*ii = new inquiry_info[max_rsp];
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, ii, flags);

	if (num_rsp < 0) {
		perror("Error [hci_inquiry]");
		return 1;
	}
	else {
		printf("Search Complete!\n");
	}

	return 0;
}