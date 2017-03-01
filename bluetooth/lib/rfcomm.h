#ifndef RFCOMM_H
#define RFCOMM_H

#include "config.h"
#include <bluetooth/rfcomm.h>

int rfcomm_send(int*, char*);
int rfcomm_receive(int*);

#endif