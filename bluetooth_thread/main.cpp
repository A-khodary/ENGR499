#include "lib/config.h"
#include "lib/scan.h"
#include "lib/rfcomm.h"
#include "lib/prompt.h"

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

using namespace std;

void print_menu();
int prompt_device(char*, const int&);

void scanBluetooth(inquiry_info *&, char*&);

void runBluetoothSend(deque<string>&, char*, int&);
void runBluetoothReceive(deque<string>&, int&);

mutex mtx;
condition_variable bt_send, bt_receive;

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	char* dest = new char[BT_ADDR_SIZE];

	// sockets for sending and recieving thread
	int& send_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	int& receive_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	try {
		scanBluetooth(ii, dest);
	}
	catch (runtime_error& ex) {
		cerr << "Exception caught: " << ex.what() << endl;
		return 1;
	}

	deque<string> msgs;

	//cout << "thead" << endl;
	thread bt_send(runBluetoothSend, std::ref(msgs), dest, send_sock);
	thread bt_receive(runBluetoothReceive, std::ref(msgs), receive_sock);

	bt_send.join();
	bt_receive.join();

	// close the 2 sockets
	close(send_sock);
	close(receive_sock);

	delete ii;
	delete dest;
	return 0;
}


void scanBluetooth(inquiry_info *&ii, char*& dest) {
	int sock;
	int max_rsp = 255, num_rsp = -1;

	// cstring because the library is written in C
	char addr[BT_ADDR_SIZE] = { 0 };
	char name[248] = { 0 };

	string input("");

	if (bt_scan(&ii, max_rsp, num_rsp, sock) != 0) {
		num_rsp = -1;
		throw runtime_error("Cannot scan");
	}

	while (num_rsp <= 0) {
		printf("No available device\n");
		num_rsp = -1;

		printf("Do you want to scan again?\n");
		printf("Enter 1 to scan again\n");
		printf("Enter anything else to quit\n");
		getline(cin, input);

		if (input[0] != '1') {
			break;
		}

		if (bt_scan(&ii, max_rsp, num_rsp, sock) != 0) {
			num_rsp = -1;
		}
	}

	for (int i = 0; i < num_rsp; i++) {
		// convert to address
		ba2str(&(ii + i)->bdaddr, addr);

		if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
			name, 0) < 0) {
			printf("\t%i. %s  [unknown]\n", i + 1, addr);
		}
		else {
			printf("\t%i. %s  %s\n", i + 1, addr, name);
		}
	}

	close(sock);

	int device_choice = prompt_device(input, num_rsp);

	ba2str(&(ii + device_choice - 1)->bdaddr, dest);
	printf("%s\n", dest);
}

void runBluetoothSend(deque<string>& msgs, char* dest, int& sock) {
	unique_lock<mutex> lck(mtx);

	//int index = 0;
	while (true) {
		if (msgs.empty()) {
			bt_send.wait(lck);
			bt_receive.notify_one();
			continue;
		}

		if (rfcomm_send(sock, dest, msgs) != 0) {
			printf("Failed: unable to send data\n");
		}
	}

	//close(sock);
}

void runBluetoothReceive(deque<string>& msgs, int& sock) {
	unique_lock<mutex> lck(mtx);

	while (true) {
		if (msgs.empty()) {
			bt_receive.wait(lck);
			bt_send.notify_one();
			continue;
		}

		if (rfcomm_receive(sock, msgs) != 0) {
			printf("Failed: unable to receive data\n");
		}
	}

	//close(sock);
}