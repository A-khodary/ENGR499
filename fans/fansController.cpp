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
	cout << "Turning fan on" << endl;
	for(int q =0; q < 100; q++) {
		gpioSetValue(GPIOs[i], on);
		usleep(onPercent*1000);     
		gpioSetValue(GPIOs[i], off);
		usleep(offPercent*1000);
	}        
	cout << "Turning fan off" << endl;
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

void powerDown() {
	for(int i = 0; i < 8; i++) {
		gpioSetValue(GPIOs[i], off);
		gpioUnexport(GPIOs[i]);
	}
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
	bool leftRobot = true;
	bool rightRobot = false;
	//Must make sure not over rotated leading to wire twist!
	int totalRotation = 0;
	
	for(i=0; i<8; i++) {
		threads[i] = thread(fanThread, i);
	}

	usleep(1000);
	
	cout << "tesing move forward" << endl;
	moveForward(50);
	usleep(1000000);
	cout << "tesing move backward" << endl;
	moveBackward(50);
	usleep(1000000);
	cout << "tesing move left" << endl;
	moveLeft(50);
	usleep(1000000);
	cout << "tesing move right" << endl;
	moveRight(50);
	usleep(1000000);
	cout << "tesing turn CW" << endl;
	turnCW(50);
	usleep(1000000);
	cout << "tesing turn CCW" << endl;
	turnCCW(50);
	usleep(1000000);
	
	//Startup movement code, if found break
		//Turn in a circle slowly, until full revolution
		while(/*imu says not full rotation? AND NOT PAST MAX ROTATION*/) {
			turnCW(50);
		}
		//If Left robot, move toward wall - stop at 3in? and slowly move sideways along it - turning as each corner is reached to move along new wall
		if(leftRobot) {
			while(/*imu says not near edge*/) {
				moveForward(50);
			}
			while (1) {
				while(/*imu says not near corner*/) {
					moveLeft(50);
				}
				while(/*imu says not quarter rotation? AND NOT PAST MAX ROTATION*/) {
					turnCW(50);
				}
			}
		}
		//If Right robot, move toward wall - stop at 3in? and slowly move sideways along it - turning as each corner is reached to move along new wall
		else if(rightRobot) {
			while(/*imu says not near edge*/) {
				moveForward(50);
			}
			while (1) {
				while(/*imu says not near corner*/) {
					moveRight(50);
				}
				while(/*imu says not quarter rotation? AND NOT PAST MAX ROTATION*/) {
					turnCCW(50);
				}
			}
		}
		//If Center robot, keep turning in circle
		else {
			while(true) {
				if(/* not past max rotation */) {
					turnCW(50);
				} else {
					turnCCW(50);	
				}
			}
		}
		
	
	

}



