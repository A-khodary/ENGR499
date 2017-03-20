#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include "../jetsonTX1GPIO/jetsonGPIO.h"
#include <thread>
#include <mutex>
#include <condition_variable>


using namespace std;

jetsonTX1GPIONumber GPIOs[8] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio186, gpio187, gpio219};

thread threads[8];
int fanData[8];
mutex fanMutex[8];
condition_variable fanCV[8];


void TurnFanOn(int milliseconds, int gpio) {
	gpioExport(GPIOs[gpio]);
	gpioSetDirection(GPIOs[gpio], outputPin);
	cout << "Turning fan " << i << " on" << endl;
	gpioSetValue(GPIOs[gpio], on);
	usleep(milliseconds);         
	cout << "Turning fan " << i << " off" << endl;
	gpioSetValue(GPIOs[gpio], off);
}


void fanThread(int i) {
	// Wait until main() sends data
	std::unique_lock<std::mutex> lock(fanMutex[i]);
    fanCV[i].wait(lock);
 
    // after the wait, we own the lock.
    std::cout << "Fan thread is turning on fan\n";
    TurnFanOn(fanData[i], i);
    std::cout << "Fan thread done\n";
 
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    fanCV[i].notify_one();
}

void wakeUpFanThread(int i, int data) {
	fanData[i] = data; //set data
	lock_guard<std::mutex> lock(fanMutex[i]);  //acquire lock
	fanCV[i].notify_one(); //notify one
}

int main() {
	int i;
	for(i=0; i<8; i++) {
		threads[i] = thread(fanThread, i);
	}
	
	//Testing all 8 fans
	cout << "Testing all 8 fans" << endl;
	for(i=0; i<8; i++) {
		wakeUpFanThread(i, 1000);
	}
	
	while(true) {
		int fanNum;
		int time;
		cout << "Please enter the number(1-8) of the fan you would like to turn on" << endl;
		cin >> fanNum;
		cout << "Please enter the number of second you would like it to trun on for" << endl;
		cin >> time;
		
		wakeUpFanThread(fanNum, time*1000);
		
		
		
	}
}



