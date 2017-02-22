#ifndef SCAN_H
#define SCAN_H

#include "config.h"

int bt_scan(inquiry_info **, int, int *, int *, int *);

int bt_scan_helper(inquiry_info **, int , int *, int *, int *, int , int);

#endif