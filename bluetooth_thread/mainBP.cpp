#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <vector>
#include <deque>
#include <bluetooth/bluetooth.h>
#include <sys/socket.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <unistd.h>
#include <bluetooth/rfcomm.h>
#include <ctime>
#include <stdlib.h>

void runBluetoothReceive();
void runBluetoothSend();

std::mutex mtx;
std::condition_variable bt_send;
std::condition_variable bt_receive;
std::deque<std::string> send_msgs;
std::deque<std::string> received_msgs;
std::vector<std::string> destinations;

int main(int argc, char **argv)
{
    std::vector<std::thread> threads;

    srand(time(nullptr));
    
    if (argc < 1) {
	std::cerr << "Error, correct format: ./<executable name> $(./getMyBtAddr.sh)" << std::endl;
    }
    
    std::string myAddr(argv[1]);
    std::cout << "My addresss is " << myAddr << std::endl;
    
    std::string s = "00:04:4B:65:C3:75";
    if ( s != myAddr ) {
	destinations.push_back(s);
    }
    
    s = "00:04:4B:66:9F:3A";
    if ( s != myAddr ) {
	destinations.push_back(s);
    }

    //Create the receive threads first
    for (unsigned int i = 0; i < destinations.size(); i++) {
	threads.push_back(std::thread(runBluetoothReceive) );
    }

    //We only need one send thread as we are broadcasting to all other robots
    threads.push_back(std::thread(runBluetoothSend));
    
    std::unique_lock<std::mutex> lck(mtx);
    std::string temp;

    //This is just for testing purposes.
    //TODO: Need a message processing thread
    while (true) {
	lck.unlock();
	std::cout << "> Say something (one word): ";
	std::cin >> temp;
	lck.lock();
	send_msgs.push_back(temp);
	
	// wake up sending thread
	bt_send.notify_one();
    }

    for ( unsigned int i=0; i<threads.size(); i++ ) {
	threads[i].join();
    }
    
    return 0;
}

int getConnection(const char* dest) {    
    sockaddr_rc addr = { 0 };
    int s, status;

    std::cout<<"Connecting to "<<dest<<std::endl;
    
    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if ( status != 0 ) {
	std::cout<<"Unable to connect to "<<dest<<std::endl;
	return status;
    }

    return s;
}

void runBluetoothSend() {
    //Get a connection for each of the destinations
    int status;
    std::vector<int> connections;

    //We need to pause the send thread to give the receive threads
    //on the other Jetsons time to get to the 'accept'
    //In addition, we found that we couldn't have two "simultaneous"
    //send connection requests at the same time. Through experimenting
    //there seems to need to be at least 4 seconds between send
    //connection requests
    int value = 5000000 + 4000000 * (rand()%10);
    std::cout<<"random sleep value is "<<value<<std::endl;
    usleep(value);
    
    for ( unsigned int i=0; i<destinations.size(); i++ ) {
	std::string dest = destinations[i];
	int socket = getConnection( dest.c_str() );
	if ( socket != -1 ) {
	    //Connection was successful
	    connections.push_back( socket );
	} else {
	    std::cout<<"Connection was not successful"<<std::endl;
	}
    }

    //Begin the send message loop
    std::unique_lock<std::mutex> lck(mtx);
    lck.unlock();
 
    while (true) {
	lck.lock();
	unsigned int size = send_msgs.size();
	for (unsigned int i = 0; i < size; i++) {
	    //Broadcast each message to the other robots
	    for ( unsigned int j=0; j<connections.size(); j++ ) {
		status = write( connections[j], (char *)send_msgs[i].c_str(), send_msgs[i].size() );
		if ( status == 0 ) {
		    std::cout<<"Unable to send message to destination "<<destinations[j]<<std::endl;
		}
	    }
	}

	//No more messages to send
	send_msgs.clear();
	bt_send.wait(lck);
	lck.unlock();
    }
}

void runBluetoothReceive() {
    //Wait for a connection request
    const int BUF_SIZE = 1024;
    int error;
    int sock;
    char buf[BUF_SIZE] = {0};
    sockaddr_rc local_address = {0}, remote_address = {0};
    
    socklen_t option = sizeof(remote_address);
    
    sock = socket( AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if ( sock == -1 ) {
	std::cout<<"Unable to get a socket"<<std::endl;
	return;
    }
    
    local_address.rc_family = AF_BLUETOOTH;
    local_address.rc_bdaddr = { { 0, 0, 0, 0, 0, 0 } };
    local_address.rc_channel = (uint8_t)0;//bind to port 0 of first available bluetooth adapter
    error = bind(sock, (struct sockaddr *)&local_address, sizeof(local_address));
    if ( error == -1 ) {
	std::cout<<"Error on bind"<<std::endl;
	return;
    }
    
    // put socket into listening mode
    error = listen(sock, 1);
    if ( error == -1 ) {
	std::cout<<"Error on listen"<<std::endl;
	return;
    }
    
    // accept one connection
    std::cout<<"About to call accept"<<std::endl;
    int client = accept(sock, (struct sockaddr *)&remote_address, &option);
    if ( client == -1 ) {
	std::cout<<"Error on accept"<<std::endl;
	return;
    }

    ba2str( &remote_address.rc_bdaddr, buf);
    std::cout<<"Accepted connection from "<< buf << std::endl;
    
    std::unique_lock<std::mutex> lck(mtx);//mtx is a global lock
    lck.unlock();

    //Start our infinite message receiving loop
    while (true) {
	memset( buf, 0, sizeof(buf) );
	int bytes_read = read( client, buf, BUF_SIZE );
	std::cout<<"byte read is "<<bytes_read<<std::endl;
	if ( bytes_read > 0 ) {
	    //Add the message to the message queue
	    lck.lock();
	    received_msgs.push_back(buf);//Add the new message to the received message queue

	    //TODO:Will eventually need a message processor thread that waits on the
	    //bt_receive
	    std::cout<<"message is:"<<buf<<std::endl;//Just for testing purposes
	    
	    bt_receive.notify_one();//Wake up the message handler that we have received a message
	    lck.unlock();
	}
    }
}
