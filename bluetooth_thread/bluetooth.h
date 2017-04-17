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

class Bluetooth {
public:
	Bluetooth();
	Bluetooth(const std::deque<std::string>&);
	Bluetooth(const std::vector<std::string>&);
	~Bluetooth();

	int send(const std::string&, const std::string&);
	int send(int, const std::string&);
	bool connect(const std::string&);
	bool connect(int);

	bool initListener(int);
	int listen(int, std::string&, char*);
	int listen(int, std::string&);

	int find(const std::string&) const;

	// accessor
	std::string& operator[](int);
	const std::string& operator[](int) const;
	int getMyAddress() const;

	// mutator
	void setMyAddress(int);
	void setMyAddress(const std::string&);
private:
	void setUp();

	int myAddr_;
	std::deque<std::string> device_addressQ_;
	int send_sock_, send_sock2_;
	RfcommReceivePackage receivePack_, receivePack2_;
};

#endif // !BLUETOOTH_H

