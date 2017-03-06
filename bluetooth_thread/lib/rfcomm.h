#ifndef RFCOMM_H
#define RFCOMM_H

#include "config.h"
#include <bluetooth/rfcomm.h>

int rfcomm_send(int &sock, char* dest, std::deque<std::string>& msgs);
int rfcomm_receive(int &sock, std::deque<std::string>& msgs);

#endif