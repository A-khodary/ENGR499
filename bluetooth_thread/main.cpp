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

void runBluetoothSend(deque<string>&, deque<string>&, char*, int&);
void runBluetoothReceive(deque<string>&, deque<string>&, int&);

mutex mtx;
condition_variable bt_send, bt_receive, main_cv;

int main(int argc, char **argv)
{
	if (argc > 1) {
		cout << "\"" << argv[1] << "\"" << endl;
	}
	string temp1;
	cout << "get args: ";
	cin << temp1 << endl;

	inquiry_info *ii = NULL;
	char* dest = new char[BT_ADDR_SIZE];
	//strcpy(dest, "00:04:4B:66:9F:3A");
	strcpy(dest, "00:04:4B:65:BB:42");

	// sockets for sending and recieving thread
	int send_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	int receive_sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	printf("Device selected: %s\n", dest);

	deque<string> send_msgs;
	deque<string> received_msgs;
	deque<string> bufferQ;

	//cout << "thead" << endl;
	thread bt_sendThread(runBluetoothSend, std::ref(send_msgs), std::ref(bufferQ), dest, std::ref(send_sock));
	thread bt_receiveThread(runBluetoothReceive, std::ref(received_msgs), std::ref(bufferQ), std::ref(receive_sock));

	unique_lock<mutex> lck(mtx);
	string temp;
	while (true) {
		lck.unlock();
		cout << "> Say something: ";
		cin >> temp;
		cout << "try to send \"" << temp << "\"" << endl;

		printf("Main: wake up sending thread\n");
		
		lck.lock();
		send_msgs.push_back(temp);
		// wake up sending thread
		bt_send.notify_one();
		cout << "waked up!!" << endl;
		temp.clear();
		//main_cv.wait(lck);
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

void runBluetoothSend(deque<string>& msgs, deque<string>& otherQ, char* dest, int& sock) {
	unique_lock<mutex> lck(mtx);
	lck.unlock();
	cout << "Thread: send begin" << endl;
	//int index = 0;
	int status = initRfcommSend(sock, dest);

	while (true) {
		printf("Thread: sending\n");
		lck.lock();
		printf("msgQ size = %d\n", (int)msgs.size());
		for (int i = 0; (unsigned)i < msgs.size(); i++) {
			//it = msgs.begin();

			status = rfcomm_send(sock, msgs[i]);
			if (status <= 0) {
				printf("Failed: unable to send data\n");

				// break the sending loop
				break;
			}
			cout << "Msg sent: \"" << msgs[i] << "\"" << endl;
		}
		msgs.clear();
		lck.unlock();

		
		//main_cv.notify_one();
		lck.lock();		
		if (msgs.empty()) {
			cout << "Thread: acquire sending lock" << endl;
			bt_send.wait(lck);
			//bt_receive.notify_one();
			//bt_send.wait(sendLock);
		}
		lck.unlock();

	}

	//close(sock);
}

void runBluetoothReceive(deque<string>& msgs, deque<string>& otherQ, int& sock) {
	unique_lock<mutex> lck(mtx);
	lck.unlock();

	cout << "Thread: receive begin" << endl;

	// initialize variables
	struct sockaddr_rc local_address = { 0 };
	struct sockaddr_rc remote_addr = { 0 };
	int client = 0;

	char buffer[1024] = { 0 };
	char buffer2[1024] = { 0 };
	ba2str(&local_address.rc_bdaddr, buffer);
	cout << "LOCAL BEFORE: " << buffer << endl;
	ba2str(&remote_addr.rc_bdaddr, buffer2);
	cout << "REMOTE BEFORE: " << buffer2 << endl;
	memset(buffer, 0, 1024);
	memset(buffer2, 0, 1024);

	// call function to initialize the receiving thread
	initRfcommReceive(local_address, remote_addr,
		client, sock);

	ba2str(&local_address.rc_bdaddr, buffer);
	cout << "LOCAL AFTER: " << buffer << endl;
	ba2str(&remote_addr.rc_bdaddr, buffer2);
	cout << "REMOTE BEFORE: " << buffer2 << endl;
	memset(buffer, 0, 1024);
	memset(buffer2, 0, 1024);

	string msg;
	while (true) {
		printf("Thread: listening: \n");
		if (rfcomm_receive(local_address, remote_addr, buffer,
			client, msg) <= 0) {
			printf("Failed: unable to receive data\n");
		}
		else {
			cout << buffer << endl;
			lck.lock();
			msgs.push_back(msg);
			lck.unlock();
		}
		msg.clear();
	}

	//close(client);
	//close(sock);
}


void exchangeMsgs(deque<string>& destQ, deque<string>& srcQ) {
	while (!srcQ.empty()) {
		destQ.push_back(srcQ[0]);
		srcQ.pop_front();
	}

}
