#include "bluetooth.h"
#include "lib/rfcomm.h"
#include "lib/config.h"

using namespace std;

Bluetooth::Bluetooth()
	: myAddr_(-1)
{
	setUp();
}

Bluetooth::Bluetooth(const deque<string>& addrs)
	: myAddr_(-1), device_addressQ_(addrs)
{
	setUp();
}
Bluetooth::Bluetooth(const vector<string>& addrs)
	: myAddr_(-1), device_addressQ_(addrs.begin(), addrs.end())
{
	setUp();
}

Bluetooth::~Bluetooth() {
	close(send_sock_);
	close(receive_sock_);

	close(send_sock2_);
	close(receive_sock2_);
}

int Bluetooth::send(string dest, string msg) {
	int index = -1;
	for (int i = 0; (unsigned)i < device_addressQ_.size(); i++) {
		if (dest == device_addressQ_[i]) {
			index = i;
			break;
		}
	}

	if (index == -1 || index == myAddr_) {
		return -1;
	}

	return send(index, msg);
}

int Bluetooth::send(int destIndex, string msg) {
	int status = -1;
	char* buffer = new char[BT_ADDR_SIZE];
	strcpy(buffer, msg.c_str());

	if (destIndex < myAddr_) {
		// send to lower index
		status = rfcomm_send(send_sock2_, buffer, msg);
	}
	else if (destIndex > myAddr_) {
		// send to higher index
		status = rfcomm_send(send_sock_, buffer, msg);
	}
	
	delete buffer;
	return status;
}

bool Bluetooth::connect(string dest) {
	if (dest.size() != (BT_ADDR_SIZE - 1)) {
		return false;
	}

	int index = -1;
	for (int i = 0; (unsigned)i < device_addressQ_.size(); i++) {
		if (dest == device_addressQ_[i]) {
			index = i;
			break;
		}
	}

	if (index == -1 || index == myAddr_) {
		return false;
	}

	int status = -1;

	char* buffer = new char[BT_ADDR_SIZE];
	strcpy(buffer, dest.c_str());

	if (index < myAddr_) {
		// connect to lower index
		status = initRfcommSend(send_sock2_, buffer);
	}
	else if (index > myAddr_) {
		// connect to higher index
		status = initRfcommSend(send_sock_, buffer);
	}

	delete buffer;
	return status == 0;
}


string& Bluetooth::operator[](int loc) {
	return device_addressQ_.at(loc);
}

const string& Bluetooth::operator[](int loc) const {
	return device_addressQ_.at(loc);
}


void Bluetooth::setUp() {
	if (device_addressQ_.size() != 3 && myAddr_ < 0) {
		return;
	}

	send_sock_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	send_sock2_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	receive_sock_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	receive_sock2_ = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}