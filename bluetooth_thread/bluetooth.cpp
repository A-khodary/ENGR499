#include "bluetooth.h"

#include <stdexcept>

using namespace std;

RfcommReceivePackage::RfcommReceivePackage() {
	local_address = { 0 };
	remote_address = { 0 };
	client = 0;
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}

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
	close(receivePack_.sock);

	close(send_sock2_);
	close(receivePack2_.sock);
}

int Bluetooth::send(const string& dest, const string& msg) {
	return send(find(dest), msg);
}

int Bluetooth::send(int index, const string& msg) {
	if (index < 0 || (unsigned)index >= device_addressQ_.size()
		|| index == myAddr_)
		return -1;

	int status = -1;

	if (index < myAddr_) {
		// send to lower index
		status = rfcomm_send(send_sock2_, msg);
	}
	else if (index > myAddr_) {
		// send to higher index
		status = rfcomm_send(send_sock_, msg);
	}

	// status <= 0: error
	// status > 0: return number of bytes written
	return status;
}

bool Bluetooth::connect(const string& dest) {
	return connect(find(dest));
}


bool Bluetooth::connect(int index) {
	if (index == -1 || index == myAddr_) {
		return false;
	}

	int status = -1;
	char* buffer = new char[BT_ADDR_SIZE];
	memset(buffer, '\0', BT_ADDR_SIZE);
	strncpy(buffer, device_addressQ_.at(index).c_str(),
		BT_ADDR_SIZE - 1);

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

bool Bluetooth::initListener(int pack_number) {
	if (pack_number == 1)
		return initRfcommReceive(receivePack_.local_address,
			receivePack_.remote_address, receivePack_.client,
			receivePack_.sock) > 0;

	if (pack_number == 2)
		return initRfcommReceive(receivePack2_.local_address,
			receivePack2_.remote_address, receivePack2_.client,
			receivePack2_.sock) > 0;

	return false;
}

int Bluetooth::listen(int pack_number, string& msg) {
	char buffer[1024];
	memset(buffer, '\0', 1024);

	return listen(pack_number, msg, buffer);
}


int Bluetooth::listen(int pack_number, string& msg, char* buffer) {
	if (pack_number == 1)
		return rfcomm_receive(receivePack_.local_address,
			receivePack_.remote_address, buffer,
			receivePack_.client, msg);

	if (pack_number == 2)
		return rfcomm_receive(receivePack2_.local_address,
			receivePack2_.remote_address, buffer,
			receivePack2_.client, msg);

	return -1;
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
}

int Bluetooth::find(const std::string& address) const {
	int index = -1;
	for (int i = 0; (unsigned)i < device_addressQ_.size(); i++) {
		if (address == device_addressQ_[i]) {
			index = i;
			break;
		}
	}

	return index;
}

int Bluetooth::getMyAddress() {
	return myAddr_;
}


void Bluetooth::setMyAddress(int myAddr) {
	if (myAddr < 0 || (unsigned)myAddr >= device_addressQ_.size())
		return;

	myAddr_ = myAddr;
}

void Bluetooth::setMyAddress(const string& address) {
	setMyAddress(find(address));
}