#ifndef SCAN_H
#define SCAN_H

#include "config.h"

#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int bt_scan(inquiry_info **ii, int max_rsp, int &num_rsp, int &sock);

int bt_scan_helper(inquiry_info **ii, int max_rsp, int &num_rsp, int &dev_id,
	int &sock, int len, int flags);

#endif