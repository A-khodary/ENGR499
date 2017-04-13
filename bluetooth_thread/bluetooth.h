#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <deque>
#include <vector>
#include <string>

class Bluetooth {
public:
	Bluetooth();
	Bluetooth(const std::deque<std::string>&);
	Bluetooth(const std::vector<std::string>&);
	~Bluetooth();

	int send(std::string, std::string);
	int send(int, std::string);

	bool connect(std::string);

	std::string& operator[](int);
	const std::string& operator[](int) const;
private:
	void setUp();

	int myAddr_;
	std::deque<std::string> device_addressQ_;
	int send_sock_, send_sock2_;
	int receive_sock_, receive_sock2_;
};

#endif // !BLUETOOTH_H

