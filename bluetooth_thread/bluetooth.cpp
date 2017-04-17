#include "bluetooth.h"

#include <stdexcept>

using namespace std;

RfcommReceivePackage::RfcommReceivePackage() {
	local_address = { 0 };
	remote_address = { 0 };
	client = 0;
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
}

RfcommSendPackage::RfcommSendPackage() {
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	currThreadNum = 0;
}

Bluetooth::Bluetooth()
	: myAddr_(-1)
{
}

Bluetooth::Bluetooth(const deque<string>& addrs)
	: myAddr_(-1), device_addressQ_(addrs)
{
}

Bluetooth::Bluetooth(const vector<string>& addrs)
	: myAddr_(-1), device_addressQ_(addrs.begin(), addrs.end())
{
}

Bluetooth::~Bluetooth() {
	close(sendPack_.sock);
	close(receivePack_.sock);

	close(sendPack2_.sock);
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
		status = rfcomm_send(sendPack2_.sock, msg);
	}
	else if (index > myAddr_) {
		// send to higher index
		status = rfcomm_send(sendPack_.sock, msg);
	}

	// status <= 0: error
	// status > 0: return number of bytes written
	return status;
}

bool Bluetooth::connect(const string& dest, int currThreadNum) {
	return connect(find(dest), currThreadNum);
}


bool Bluetooth::connect(int index, int currThreadNum) {
	if (index < 0 || (unsigned)index >= device_addressQ_.size()
		|| index == myAddr_) {
		return false;
	}

	int status = -1;
	char* buffer = new char[BT_ADDR_SIZE];
	memset(buffer, '\0', BT_ADDR_SIZE);
	strncpy(buffer, device_addressQ_.at(index).c_str(),
		BT_ADDR_SIZE - 1);

	if (index < myAddr_) {
		// connect to lower index
		status = initRfcommSend(sendPack2_.sock, buffer);
		sendPack2_.currThreadNum = currThreadNum;
	}
	else if (index > myAddr_) {
		// connect to higher index
		status = initRfcommSend(sendPack_.sock, buffer);
		sendPack_.currThreadNum = currThreadNum;
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


int Bluetooth::getCurrTheradNum(const std::string& dest) const {
	return getCurrTheradNum(find(dest));
}

// @param index: index of the target device
int Bluetooth::getCurrTheradNum(int index) const {
	if (index < 0 || (unsigned)index >= device_addressQ_.size()
		|| index == myAddr_) {
		return false;
	}

	if (index < myAddr_) {
		// connect to lower index
		return sendPack2_.currThreadNum;
	}
	else if (index > myAddr_) {
		// connect to higher index
		return sendPack_.currThreadNum;
	}

	return -1;
}

int Bluetooth::getMyAddress() const {
	return myAddr_;
}

void Bluetooth::setMyAddress(const string& address) {
	setMyAddress(find(address));
}

void Bluetooth::setMyAddress(int myAddr) {
	if (myAddr < 0 || (unsigned)myAddr >= device_addressQ_.size())
		return;

	myAddr_ = myAddr;
}