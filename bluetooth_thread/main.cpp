#include "lib/config.h"
#include "lib/scan.h"
#include "lib/rfcomm.h"
#include "lib/prompt.h"

#include <iostream>
#include <thread>
#include <functional>

using namespace std;

void print_menu();
int prompt_device(char*, const int&);
void scanBluetooth(inquiry_info *&, char*&);
void runBluetoothSend(char*, deque<string>&);
void runBluetoothReceive(deque<string>&);

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	char* dest = new char[BT_ADDR_SIZE];

	scanBluetooth(ii, dest);

	deque<string> msgs;

	thread bt_send(runBluetoothSend, dest, std::ref(msgs));
	//thread bt_receive(runBluetoothReceive, std::ref(msgs));

	bt_send.join();
	//bt_receive.join();

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

void runBluetoothSend(char* dest, deque<string>& msgs) {
	// allocate a socket
	int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	//int index = 0;
	while (true) {
		if (rfcomm_send(sock, dest, msgs) != 0) {
			printf("Failed: unable to send data\n");
		}
	}

	close(sock);
}

void runBluetoothReceive(deque<string>& msgs) {
	// allocate a socket
	int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	while (true) {
		if (rfcomm_receive(sock, msgs) != 0) {
			printf("Failed: unable to receive data\n");
		}
	}

	close(sock);
}