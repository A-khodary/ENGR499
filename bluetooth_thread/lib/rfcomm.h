#ifndef RFCOMM_H
#define RFCOMM_H

#include "config.h"
#include <bluetooth/rfcomm.h>

int initRfcommSend(int& sock, char* dest);
int rfcomm_send(int &sock, char* dest, std::string& msg);

void initRfcommReceive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	bdaddr_t my_bdaddr_any,
	socklen_t opt,
	int& client,
	int& sock);

int rfcomm_receive(struct sockaddr_rc& local_address,
	struct sockaddr_rc& remote_addr,
	bdaddr_t my_bdaddr_any,
	char* buf,
	socklen_t opt,
	int& client,
	int& sock,
	std::deque<std::string>& msgs);

#endif
