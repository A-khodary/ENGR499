#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "lib/config.h"
#include "lib/rfcomm.h"

#include <deque>
#include <vector>
#include <string>


struct RfcommReceivePackage {
	struct sockaddr_rc local_address;
	struct sockaddr_rc remote_address;
	int client;
	int sock;

	RfcommReceivePackage();
};

struct RfcommSendPackage {
	int sock;
	int currThreadNum;

	RfcommSendPackage();
};

class Bluetooth {
public:
	Bluetooth();
	Bluetooth(const std::deque<std::string>&);
	Bluetooth(const std::vector<std::string>&);
	~Bluetooth();

	int send(const std::string&, const std::string&);
	int send(int, const std::string&);
	bool connect(const std::string&, int);
	bool connect(int, int);

	bool initListener(int);
	int listen(int, std::string&, char*);
	int listen(int, std::string&);

	int find(const std::string&) const;
	int getCurrTheradNum(const std::string&) const;
	int getCurrTheradNum(int) const;

	// accessor
	std::string& operator[](int);
	const std::string& operator[](int) const;
	int getMyAddress() const;

	// mutator
	void setMyAddress(const std::string&);
	void setMyAddress(int);
private:
	int myAddr_;
	std::deque<std::string> device_addressQ_;
	RfcommSendPackage sendPack_, sendPack2_;
	RfcommReceivePackage receivePack_, receivePack2_;
};

#endif // !BLUETOOTH_H

