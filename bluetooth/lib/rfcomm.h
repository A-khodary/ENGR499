#ifndef RFCOMM_H
#define RFCOMM_H

#include "config.h"
#include <bluetooth/rfcomm.h>

int rfcomm_send(char*);
int rfcomm_receive();

#endif