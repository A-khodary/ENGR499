#include "../lib/rfcomm.h"

int rfcomm_send(int &sock, char* dest, std::deque<std::string>& msgs)
{
	struct sockaddr_rc addr = { 0 };
	int status;

	// set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	// set to 0, binds the socket to the first available port
	addr.rc_channel = (uint8_t)1;
	str2ba(dest, &addr.rc_bdaddr);

	// connect to server
	status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));

	// send a message
	std::deque<std::string>::iterator it;
	for (int i = 0, size = msgs.size(); status == 0 && i < size; i++) {
		it = msgs.begin();
		status = write(sock, it->c_str(), it->size() + 1);

		msgs.pop_front();
	}

	if (status < 0) {
		perror("Error");
		return 1;
	}

	msgs.clear();
	return 0;
}