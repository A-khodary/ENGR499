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
//int prompt_device(char*, const int&);
void exchangeMsgs(deque<string>&, deque<string>&);

void scanBluetooth(inquiry_info *&, char*&);

void runBluetoothSend(deque<string>&, deque<string>&, char*, int&);
void runBluetoothReceive(deque<string>&, deque<string>&, int&);

mutex mtx;
condition_variable bt_send, bt_receive;

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	char* dest = new char[BT_ADDR_SIZE];
	strcpy(dest, "00:04:4B:66:9F:3A");
	// strcpy(dest, "00:04:4B:65:BB:42");

	// sockets for sending and recieving thread
	int send_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	int receive_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	/*try {
		scanBluetooth(ii, dest);
	}
	catch (runtime_error& ex) {
		cerr << "Exception caught: " << ex.what() << endl;
		return 1;
	}*/
	printf("Device selected: %s\n", dest);

	deque<string> send_msgs;
	deque<string> received_msgs;
	deque<string> bufferQ;

	//cout << "thead" << endl;
	thread bt_sendThread(runBluetoothSend, std::ref(send_msgs), std::ref(bufferQ), dest, std::ref(send_sock));
	thread bt_receiveThread(runBluetoothReceive, std::ref(received_msgs), std::ref(bufferQ), std::ref(receive_sock));

	string temp;
	while (true) {
		cin >> temp;
		send_msgs.push_back(temp);

		printf("Main: wake up sending thread\n");
		// wake up sending thread
		bt_send.notify_one();
		temp.clear();
	}

	bt_sendThread.join();
	bt_receiveThread.join();

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

	if (device_choice > 0) {
		ba2str(&(ii + device_choice - 1)->bdaddr, dest);
		printf("%s\n", dest);
	}
	else {
		printf("invalid choice: %d\n", device_choice);
	}
}

void runBluetoothSend(deque<string>& msgs, deque<string>& otherQ, char* dest, int& sock) {
	unique_lock<mutex> lck(mtx);

	//int index = 0;
	int status = 0;
	while (true) {
		for (int i = 0, size = msgs.size(); i < size; i++) {
			//it = msgs.begin();

			status = rfcomm_send(sock, dest, msgs[0]);
			if (status != 0) {
				printf("Failed: unable to send data\n");

				// break the sending loop
				break;
			}
			cout << "Msg sent: \"" << msg[i] << "\"" << endl;
			msgs.pop_front();
		}

		if (msgs.empty()) {
			cout << "Thread: acquire sending lock" << endl;
			bt_send.wait(lck);
			//bt_receive.notify_one();
		}

	}

	//close(sock);
}

void runBluetoothReceive(deque<string>& msgs, deque<string>& otherQ, int& sock) {
	unique_lock<mutex> lck(mtx);

	// initialize variables
	struct sockaddr_rc local_address = { 0 };
	struct sockaddr_rc remote_addr = { 0 };
	int client;

	socklen_t opt = sizeof(remote_addr);
	bdaddr_t my_bdaddr_any = { { 0, 0, 0, 0, 0, 0 } };

	char buffer[1024] = { 0 };

	// call function to initialize the receiving thread
	initRfcommReceive(local_address, remote_addr,
		my_bdaddr_any, sock);

	while (true) {
		printf("Thread: listening: \n");
		if (rfcomm_receive(local_address, remote_addr,
			my_bdaddr_any, buffer, opt, client
			, sock, msgs) != 0) {
			printf("Failed: unable to receive data\n");
		}
	}

	close(client);
	//close(sock);
}


void exchangeMsgs(deque<string>& destQ, deque<string>& srcQ) {
	while (!srcQ.empty()) {
		destQ.push_back(srcQ[0]);
		srcQ.pop_front();
	}

}