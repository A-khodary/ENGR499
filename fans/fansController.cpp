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
#include <signal.h>



using namespace std;

// gpio36 = pin 32, gpio37 = pin 37, gpio38 = pin 13, gpio63 = pin 33, gpio184 = pin 18, gpio186 = pin 31, gpio187 = pin 37, gpio219 = pin 29
jetsonTX1GPIONumber GPIOs[8] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio186, gpio187, gpio219};

thread threads[8];
int fanData[8];
mutex fanMutex[8];
condition_variable fanCV[8];

void TurnFanOn(int onPercent, int i) {
    int offPercent = 100-onPercent;
	gpioExport(GPIOs[i]);
	gpioSetDirection(GPIOs[i], outputPin);
	for(int q =0; q < 100; q++) {
		gpioSetValue(GPIOs[i], on);
		usleep(onPercent*1000);     
		gpioSetValue(GPIOs[i], off);
		usleep(offPercent*1000);
	}        
	gpioUnexport(GPIOs[i]);
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

void powerDown(int sig) {
	for(int i = 0; i < 8; i++) {
		gpioSetValue(GPIOs[i], off);
		gpioUnexport(GPIOs[i]);
	}
	exit(1);
}

void moveForward(int onPercent) {
	fanData[0] = onPercent;
	fanData[5] = onPercent;
	wakeUpFanThread(0);
	wakeUpFanThread(5);
}
void moveBackward(int onPercent) {
	fanData[1] = onPercent;
	fanData[4] = onPercent;
	wakeUpFanThread(1);
	wakeUpFanThread(4);
}
void moveLeft(int onPercent) {
	fanData[3] = onPercent;
	fanData[6] = onPercent;
	wakeUpFanThread(3);
	wakeUpFanThread(6);
}
void moveRight(int onPercent) {
	fanData[2] = onPercent;
	fanData[7] = onPercent;
	wakeUpFanThread(2);
	wakeUpFanThread(7);
}

void turnCCW(int onPercent) {
	fanData[0] = onPercent;
	fanData[2] = onPercent;
	fanData[4] = onPercent;
	fanData[6] = onPercent;
	wakeUpFanThread(0);
	wakeUpFanThread(2);
	wakeUpFanThread(4);
	wakeUpFanThread(6);
}
void turnCW(int onPercent) {
	fanData[1] = onPercent;
	fanData[3] = onPercent;
	fanData[5] = onPercent;
	fanData[7] = onPercent;
	wakeUpFanThread(1);
	wakeUpFanThread(3);
	wakeUpFanThread(5);
	wakeUpFanThread(7);
} 

int main() {
	int i;
	bool leftRobot = true;
	bool rightRobot = false;
	//Must make sure not over rotated leading to wire twist!
	int totalRotation = 0;
	
	for(i=0; i<8; i++) {
		threads[i] = thread(fanThread, i);
	}

	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = powerDown;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, 0);


	usleep(5000000);

	int onPercent = 10;

	cout << "tesing move forward" << endl;
	moveForward(onPercent);
	usleep(10000000);
	cout << "tesing move backward" << endl;
	moveBackward(onPercent);
	usleep(10000000);
	cout << "tesing move left" << endl;
	moveLeft(onPercent);
	usleep(10000000);
	cout << "tesing move right" << endl;
	moveRight(onPercent);
	usleep(10000000);
	cout << "tesing turn CW" << endl;
	turnCW(onPercent);
	usleep(10000000);
	cout << "tesing turn CCW" << endl;
	turnCCW(onPercent);
	usleep(10000000);
	

while(1) {}
		
	
	

}



