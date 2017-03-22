#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include "jetsonGPIO.h"
#include <thread>
#include <mutex>
#include <condition_variable>


using namespace std;

jetsonTX1GPIONumber GPIOs[8] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio186, gpio187, gpio219};

thread threads[8];
int fanData[8];
mutex fanMutex[8];
condition_variable fanCV[8];

void TurnFanOn(int onPercent, int gpio) {
    int offPercent = 100-onPercent;
	gpioExport(GPIOs[gpio]);
	gpioSetDirection(GPIOs[gpio], outputPin);
	cout << "Turning fan on" << endl;
	for(int q =0; q < 100; q++) {
		gpioSetValue(GPIOs[gpio], on);
		usleep(onPercent*1000);     
		gpioSetValue(GPIOs[gpio], off);
		usleep(offPercent*1000);
	}        
	cout << "Turning fan off" << endl;
	gpioUnexport(GPIOs[gpio]);
}

void fanThread(int i) {
    while(1) {
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
}

void wakeUpFanThread(int i) {
	lock_guard<std::mutex> lock(fanMutex[i]);  //acquire lock
	fanCV[i].notify_one(); //notify one
}


void moveForward(int onPercent) {
	fanData[1] = onPercent;
	fanData[6] = onPercent;
	wakeUpFanThread(1);
	wakeUpFanThread(6);
}
void moveBackward(int onPercent) {
	fanData[2] = onPercent;
	fanData[5] = onPercent;
	wakeUpFanThread(2);
	wakeUpFanThread(5);
}
void moveLeft(int onPercent) {
	fanData[4] = onPercent;
	fanData[7] = onPercent;
	wakeUpFanThread(4);
	wakeUpFanThread(7);
}
void moveRight(int onPercent) {
	fanData[3] = onPercent;
	fanData[8] = onPercent;
	wakeUpFanThread(3);
	wakeUpFanThread(8);
}


void turnCCW(int onPercent) {
	fanData[1] = onPercent;
	fanData[3] = onPercent;
	fanData[5] = onPercent;
	fanData[7] = onPercent;
	wakeUpFanThread(1);
	wakeUpFanThread(3);
	wakeUpFanThread(5);
	wakeUpFanThread(7);
}
void turnCW(int onPercent) {
	fanData[2] = onPercent;
	fanData[4] = onPercent;
	fanData[6] = onPercent;
	fanData[8] = onPercent;
	wakeUpFanThread(2);
	wakeUpFanThread(4);
	wakeUpFanThread(6);
	wakeUpFanThread(8);
} 


int main() {
	int i;
	for(i=0; i<8; i++) {
		threads[i] = thread(fanThread, i);
	}
	//Testing all 8 fans
	cout << "Testing all 8 fans" << endl;
	
	for(i=0; i<8; i++) {
		wakeUpFanThread(i);
	}
	
	cout << "tesing move forward" << endl;
	moveForward(50);
	
	cout << "tesing move backward" << endl;
	moveBackward(50);
	
	cout << "tesing move left" << endl;
	moveLeft(50);
	
	cout << "tesing move right" << endl;
	moveRight(50);
	
	cout << "tesing turn CW" << endl;
	turnCW(50);
	
	cout << "tesing turn CCW" << endl;
	turnCCW(50);
	
	while(true) {
		int fanNum;
		int time;
		cout << "Please enter the number(1-8) of the fan you would like to turn on" << endl;
		cin >> fanNum;
		
		wakeUpFanThread(fanNum);
		
	
	}

}



