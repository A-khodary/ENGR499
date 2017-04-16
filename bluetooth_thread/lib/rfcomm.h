#ifndef RFCOMM_H
#define RFCOMM_H

#include "config.h"
#include <bluetooth/rfcomm.h>

int initRfcommSend(int& sock, char* dest);
int rfcomm_send(int &sock, const std::string& msg);

int initRfcommReceive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	int& client,
	int& sock);

int rfcomm_receive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	char* buffer,
	int& client,
	std::string& msg);

#endif
