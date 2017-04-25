//#include "jetsonGPIO.h"
extern "C" {
#include "jetsonGPIO.h"
}

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdlib>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <signal.h>

// 1 = pin 32, 2 = pin 16, 3 = pin 13, 4 = pin 33, 5 = pin 18, 6 = pin 31, 7 = pin 37, 8 = pin 29
jetsonTX1GPIONumber GPIOs[6] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio187};

std::thread threads[6];
int power[6];
std::mutex signalMutex[6];
std::condition_variable signalCV[6];

void TurnSignalOn(int onPercent, int i) {
    int offPercent = 100-onPercent;
    gpioExport(GPIOs[i]);
    gpioSetDirection(GPIOs[i], outputPin);
    for(int q =0; q < 100; q++) {
	gpioSetValue(GPIOs[i], on);
	usleep(onPercent*100);     
	gpioSetValue(GPIOs[i], off);
	usleep(offPercent*100);
    }        
    gpioUnexport(GPIOs[i]);
}

void TurnOffGPIO(int i) {
    gpioSetValue(GPIOs[i], off);
    gpioUnexport(GPIOs[i]);
}

void signalThread(int i) {
    while(1) {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lock(signalMutex[i]);
		signalCV[i].wait(lock);
		if(power[i] < 0) {
			TurnOffGPIO(i);
		}
		// after the wait, we own the lock.
		TurnSignalOn(power[i], i);
		// Manual unlocking is done before notifying, to avoid waking up
		// the waiting thread only to block again (see notify_one for details)
		lock.unlock();
		signalCV[i].notify_one();
    }
}

void wakeUpSignalThread(int i) {
    std::lock_guard<std::mutex> lock(signalMutex[i]);  //acquire lock
    signalCV[i].notify_one(); //notify one
}

//TODO: fine tune stopping? let controller deal with it?
void moveForward(int onPercent) {
    power[0] = onPercent;
    power[5] = onPercent;
    wakeUpSignalThread(0);
    wakeUpSignalThread(5);
}
void moveBackward(int onPercent) {
    power[1] = onPercent;
    power[4] = onPercent;
    wakeUpSignalThread(1);
    wakeUpSignalThread(4);
}
void moveLeft(int onPercent) {
    power[3] = onPercent;
    power[6] = onPercent;
    wakeUpSignalThread(3);
    wakeUpSignalThread(6);
}
void moveRight(int onPercent) {
    power[2] = onPercent;
    power[7] = onPercent;
    wakeUpSignalThread(2);
    wakeUpSignalThread(7);
}
void turnCCW(int onPercent) {
    power[0] = onPercent;
    power[4] = onPercent;
    wakeUpSignalThread(0);
    wakeUpSignalThread(4);
}
void turnCW(int onPercent) {
    power[1] = onPercent;
    power[5] = onPercent;
    wakeUpSignalThread(1);
    wakeUpSignalThread(5);
} 

void moveX(int onPercent) {
    if(onPercent < 0) {
	moveBackward(onPercent);
    }
    else if (onPercent > 0) {
	moveForward(abs(onPercent));
    }
}
void moveY(int onPercent) {
    if(onPercent < 0) {
	moveLeft(abs(onPercent));
    }
    else  if (onPercent > 0) {
	moveRight(abs(onPercent));
    }
}
void rotate(int onPercent) {
	if(onPercent < 0) {
		turnCCW(onPercent);
	} else if(onPercent > 0) {
		turnCW(abs(onPercent));
	}
}

void FanExecution() {
    int i;
	std::count << "Starting signal threads, and testing all 6" << std::endl;
    for(i=0; i<6; i++) {
		threads[i] = std::thread(signalThread, i);
    }
	int onPercent = 50;
	std::cout << "moveForward at 50%" << std::endl;
	moveForward(onPercent);
	usleep(1000000);
	std::cout << "moveBackward at 50%" << std::endl;
	moveBackward(onPercent);
	usleep(1000000);
	std::cout << "moveLeft at 50%" << std::endl;
	moveLeft(onPercent);
	usleep(1000000);
	std::cout << "moveRight at 50%" << std::endl;
	moveRight(onPercent);
	usleep(1000000);
	std::cout << "turnCW at 50%" << std::endl;
	turnCW(onPercent);
	usleep(1000000);
	std::cout << "turnCCW at 50%" << std::endl;
	turnCCW(onPercent);
	usleep(1000000);
	

	while(1) {}

}

