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

// 1 = pin 32, 2 = pin 37, 3 = pin 13, 4 = pin 33, 5 = pin 18, 6 = pin 31, 7 = pin 37, 8 = pin 29
jetsonTX1GPIONumber GPIOs[8] = {gpio36, gpio37, gpio38, gpio63, gpio184, gpio186, gpio187, gpio219};

std::thread threads[8];
int fanPower[8];
int fanTime[8];
std::mutex fanMutex[8];
std::condition_variable fanCV[8];
bool MaxRotationCCW, MaxRotationCW;


void checkRotation(){
    //Get info from imu!!!
    MaxRotationCCW = false;
    MaxRotationCW = false;
}

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
    signal(SIGINT, SIG_IGN);
    while(1) {
	// Wait until main() sends data
	std::unique_lock<std::mutex> lock(fanMutex[i]);
	fanCV[i].wait(lock);
	 
	if(fanPower[i] < 0) {
	    TurnFanOff(i);
	}
	// after the wait, we own the lock.
	//std::cout << "Fan thread is turning on fan\n";
	TurnFanOn(fanPower[i], i);
	//std::cout << "Fan thread done\n";
	 
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
    exit(1);
}

void moveForward(int onPercent) {
    //go
    fanPower[0] = onPercent;
    fanPower[5] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(5);
    usleep(1000000);
    //stop
    fanPower[1] = onPercent;
    fanPower[4] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(4);
}
void moveBackward(int onPercent) {
    //go
    fanPower[1] = onPercent;
    fanPower[4] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(4);
    usleep(1000000);
    //stop
    fanPower[0] = onPercent;
    fanPower[5] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(5);
}
void moveLeft(int onPercent) {
    //go
    fanPower[3] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(3);
    wakeUpFanThread(6);
    usleep(1000000);
    //stop
    fanPower[2] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(2);
    wakeUpFanThread(7);
}
void moveRight(int onPercent) {
    //go
    fanPower[2] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(2);
    wakeUpFanThread(7);
    usleep(1000000);
    //stop
    fanPower[3] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(3);
    wakeUpFanThread(6);
}

void turnCCW(int onPercent) {
    //go
    fanPower[0] = onPercent;
    fanPower[2] = onPercent;
    fanPower[4] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(2);
    wakeUpFanThread(4);
    wakeUpFanThread(6);
    usleep(1000000);
    //stop
    fanPower[1] = onPercent;
    fanPower[3] = onPercent;
    fanPower[5] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(3);
    wakeUpFanThread(5);
    wakeUpFanThread(7);
}
void turnCW(int onPercent) {
    //go
    fanPower[1] = onPercent;
    fanPower[3] = onPercent;
    fanPower[5] = onPercent;
    fanPower[7] = onPercent;
    wakeUpFanThread(1);
    wakeUpFanThread(3);
    wakeUpFanThread(5);
    wakeUpFanThread(7);
    usleep(1000000);
    //stop
    fanPower[0] = onPercent;
    fanPower[2] = onPercent;
    fanPower[4] = onPercent;
    fanPower[6] = onPercent;
    wakeUpFanThread(0);
    wakeUpFanThread(2);
    wakeUpFanThread(4);
    wakeUpFanThread(4);
    wakeUpFanThread(6);
} 

void FanExecution() {
    int i;
    bool leftRobot = true;
    bool rightRobot = false;

	
    for(i=0; i<8; i++) {
	threads[i] = std::thread(fanThread, i);
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = powerDown;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);


    usleep(2000000);

    int onPercent = 20;

    /*
      std::cout << "tesing move forward" << std::endl;
      moveForward(onPercent);
      usleep(10000000);
      std::cout << "tesing move backward" << std::endl;
      moveBackward(onPercent);
      usleep(10000000);
      std::cout << "tesing move left" << std::endl;
      moveLeft(onPercent);
      usleep(10000000);
      std::cout << "tesing move right" << std::endl;
      moveRight(onPercent);
      usleep(10000000);
      std::cout << "tesing turn CW" << std::endl;
      turnCW(onPercent);
      usleep(10000000);
      std::cout << "tesing turn CCW" << std::endl;
      turnCCW(onPercent);
      usleep(10000000);
    */
	
    //Turn in a circle slowly, until full revolution
    std::cout << "Start test" << std::endl;
    int testing = 0;
    checkRotation();
    while(testing < 4/*imu says not full rotation?*/) {
	std::cout << "Rotating!" << std::endl;
	if(MaxRotationCCW) {
	    turnCW(onPercent);
	} else {
	    turnCCW(onPercent);
	}
	testing++;
    }
    // move toward wall
    while(testing < 7/*imu says not near edge*/) {
	std::cout << "Moving toward edge" << std::endl;
	moveForward(onPercent);
	testing++;
    }
    // slowly move sideways along wall, turning as each corner is reached to move along new wall
    while(testing < 11/*imu says not near corner*/) {
	std::cout << "Moving toward corner" << std::endl;
	moveRight(onPercent);
	testing++;
    }
    checkRotation();
    while(testing < 13/*imu says not quarter rotation? */) {
	std::cout << "Rotating" << std::endl;
	if(MaxRotationCCW) {
	    turnCW(onPercent);
	} else {
	    turnCCW(onPercent);	
	}
	testing++;
    }
	
    std::cout << "All done!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	
    while(1) {}
}

