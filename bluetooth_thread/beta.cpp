#include "lib/config.h"
#include "lib/scan.h"
#include "lib/rfcomm.h"
#include "lib/prompt.h"
#include "bluetooth.h"

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

using namespace std;

void print_menu();
void exchangeMsgs(deque<string>&, deque<string>&);

void runBluetoothSend(deque<string>&, char*, Bluetooth&, int);
void runBluetoothReceive(deque<string>&, Bluetooth&, int);

mutex mtx;
condition_variable bt_send, bt_send2, main_cv;

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	char* dest = new char[BT_ADDR_SIZE];

	deque<string> deviceBtAddrQ;
	deviceBtAddrQ.push_back("00:04:4B:66:9F:3A");
	deviceBtAddrQ.push_back("00:04:4B:65:BB:42");
	deviceBtAddrQ.push_back("00:00:00:00:00:00");

	// check your own address
	// do corresponding init work
	Bluetooth bluetooth(deviceBtAddrQ);

	deque<string> send_msgs;
	deque<string> received_msgs;
	deque<string> bufferQ;

	//cout << "thead" << endl;
	thread bt_sendThread(runBluetoothSend, ref(send_msgs), dest,
		ref(bluetooth), 1);
	thread bt_sendThread2(runBluetoothSend, ref(send_msgs), dest,
		ref(bluetooth), 2);

	thread bt_receiveThread(runBluetoothReceive, ref(received_msgs),
		ref(bluetooth), 1);
	thread bt_receiveThread2(runBluetoothReceive, ref(received_msgs),
		ref(bluetooth), 2);

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
	bt_sendThread2.join();
	bt_receiveThread2.join();

	delete ii;
	delete dest;
	return 0;
}

void runBluetoothSend(deque<string>& msgs, char* dest,
	Bluetooth& bluetooth, int threadNum)
{
	condition_variable &bt_sendRef = threadNum == 1 ? bt_send : bt_send2;
	unique_lock<mutex> lck(mtx);
	lck.unlock();
	cout << "Thread: send begin" << endl;

	int index = bluetooth.find(string(dest));
	if (index > bluetooth.getMyAddress()) {
		// connect to lower index
		bt_sendRef.wait(lck);
	}
	cout << boolalpha << "connect to " << dest << "....\nSuccessful? "
		<< bluetooth.connect(index) << endl;


	string msg;
	msg.push_back('0' + bluetooth.getMyAddress());
	if (bluetooth.send(index, msg) <= 0) {
		// notify the device
		cerr << "Failed: unable to send \"" << msg << "\""
			<< endl;
	}

	while (true) {
		printf("Thread: sending\n");
		lck.lock();
		printf("msgQ size = %d\n", (int)msgs.size());
		for (int i = 0; (unsigned)i < msgs.size(); i++) {
			//it = msgs.begin();

			//status = rfcomm_send(sock, dest, msgs[0]);
			if (bluetooth.send(index, msgs[i]) <= 0) {
				cout << "Failed: unable to send \"" << msgs[i] << "\""
					<< endl;

				// break the sending loop
				break;
			}
			cout << "Msg sent: \"" << msgs[i] << "\"" << endl;
		}
		msgs.clear();
		/*lck.unlock();

		lck.lock();*/
		if (msgs.empty()) {
			cout << "Thread: acquire sending lock" << endl;
			bt_sendRef.wait(lck);
		}
		lck.unlock();
	}
}

void runBluetoothReceive(deque<string>& msgs, Bluetooth& bluetooth,
	int threadNum)
{
	unique_lock<mutex> lck(mtx);
	lck.unlock();
	cout << "Thread: receive begin" << endl;

	if (bluetooth.initListener(threadNum) == false) {
		cerr << "invalid threadNum = " << threadNum << endl;
		return;
	}

	char buffer[1024];
	memset(buffer, '\0', 1024);

	string msg;
	while (bluetooth.listen(threadNum, msg, buffer) <= 0) {
		printf("Failed: unable to receive data\n");
	}

	int confirmedDeviceIndex = -1;
	try {
		confirmedDeviceIndex = stoi(msg);
	}
	catch (invalid_argument ex) {
		cerr << "confirmation message \"" << msg << "\" is invalid" << endl;
		return;
	}
	catch (out_of_range ex) {
		cerr << "confirmation message too large" << endl;
		return;
	}

	if (confirmedDeviceIndex < 0 || confirmedDeviceIndex >= 3)
		return;
	condition_variable *bt_sendRef = NULL;
	confirmedDeviceIndex = bluetooth.getMyAddress() - confirmedDeviceIndex;
	if (confirmedDeviceIndex > 0) {
		// myAddr = 3, index == 1: myAddr - index = 3 - 1 = 2, 2 % 2 = 0 -> bt_send
		// myAddr = 3, index == 2: myAddr - index = 3 - 2 = 1, 1 % 2 = 1 -> bt_send2
		// myAddr = 2, index == 1: myAddr - index = 2 - 1 = 1, 1 % 2 = 1 -> bt_send2
		if (confirmedDeviceIndex % 2 == 1) {
			bt_sendRef = &bt_send2;
		}
		else {
			bt_sendRef = &bt_send;
		}
	}
	if (bt_sendRef != NULL) {
		bt_sendRef->notify_one();
	}

	msg.clear();
	while (true) {
		printf("Thread: listening: \n");
		if (bluetooth.listen(threadNum, msg, buffer) <= 0) {
			printf("Failed: unable to receive data\n");
		}
		else {
			cout << buffer << endl;
			lck.lock();
			msgs.push_back(msg);
			lck.unlock();
		}
		// clear the string
		msg.clear();
	}
}


void exchangeMsgs(deque<string>& destQ, deque<string>& srcQ) {
	while (!srcQ.empty()) {
		destQ.push_back(srcQ[0]);
		srcQ.pop_front();
	}

}
