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
#include <thread>
#include <mutex>
#include <condition_variable>
#include <signal.h>

// 1 = pin 32, 2 = pin 16, 3 = pin 13, 4 = pin 33, 5 = pin 18, 6 = pin 31, 7 = pin 37, 8 = pin 29
jetsonTX1GPIONumber GPIOs[8] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio186, gpio187, gpio219};

std::thread threads[8];
int fanPower[8];
std::mutex fanMutex[8];
std::condition_variable fanCV[8];

void TurnFanOn(int onPercent, int i) {
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

void TurnFanOff(int i) {
    gpioSetValue(GPIOs[i], off);
    gpioUnexport(GPIOs[i]);
}

void fanThread(int i) {
    //signal(SIGINT, SIG_IGN);
    while(1) {
		// Wait until main() sends data
		std::unique_lock<std::mutex> lock(fanMutex[i]);
		fanCV[i].wait(lock);
		if(fanPower[i] < 0) {
			TurnFanOff(i);
		}
		// after the wait, we own the lock.
		TurnFanOn(fanPower[i], i);
		 
		// Manual unlocking is done before notifying, to avoid waking up
		// the waiting thread only to block again (see notify_one for details)
		lock.unlock();
		fanCV[i].notify_one();
    }
}

void wakeUpFanThread(int i) {
    std::lock_guard<std::mutex> lock(fanMutex[i]);  //acquire lock
    fanCV[i].notify_one(); //notify one
}

void powerDown(int sig) {
    std::cout << "power down function!" << std::endl;
    for(int i = 0; i < 8; i++) {
		fanPower[i] = -1;
		wakeUpFanThread(i);
		threads[i].join();
		std::cout << "i = " << i << std::endl;
    }
	std::terminate();
    exit(1);
}
//TODO: fine tune stopping? let controller deal with it?
void moveForward(int onPercent) {
    fanPower[0] = onPercent;
    fanPower[5] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(5);
}
void moveBackward(int onPercent) {
    fanPower[1] = onPercent;
    fanPower[4] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(4);
}
void moveLeft(int onPercent) {
    fanPower[3] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(3);
    wakeUpFanThread(6);
}
void moveRight(int onPercent) {
    fanPower[2] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(2);
    wakeUpFanThread(7);
}
void turnCCW(int onPercent) {
    fanPower[0] = onPercent;
    fanPower[2] = onPercent;
    fanPower[4] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(2);
    wakeUpFanThread(4);
    wakeUpFanThread(6);
}
void turnCW(int onPercent) {
    fanPower[1] = onPercent;
    fanPower[3] = onPercent;
    fanPower[5] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(3);
    wakeUpFanThread(5);
    wakeUpFanThread(7);
} 

void moveX(int onPercent) {
	if(onPercent < 0) {
		moveBackward(onPercent);
	}
	else if (onPercent > 0) {
		moveForward(onPercent);
	}
}

void moveY(int onPercent) {
	if(onPercent < 0) {
		moveLeft(onPercent);
	}
	else  if (onPercent > 0) {
		moveRight(onPercent);
	}
}

void rotate(int onPercent) {
	//Max rotation amount? Deal with in controller?
	if(onPercent < 0) {
		turnCCW(onPercent);
	} else if(onPercent > 0) {
		turnCW(onPercent);
	}
}

void FanExecution() {
    int i;
	std::count << "Starting fan threads, and testing all 8" << std::endl;
    for(i=0; i<8; i++) {
		threads[i] = std::thread(fanThread, i);
    }
	for(i=0;i<8;i++) {
		std::cout << "Turning in fan " + i + " at 50%" << std::endl;
		fanPower[i] = 50;
		wakeUpFanThread(i);
		usleep(2000000);
	}

	//Will return to try and solve ctrl c or other quit command later, when we figure out combining everything?
	/*
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = powerDown;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);
	*/

    usleep(1000000);
	while(1) {}
    std::cout << "All done!!!!!!!!!!!!!!!!!!!!!" << std::endl;

}

