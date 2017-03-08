#include <iostream>
#include <cstdlib>
#include <thread>
#include "jetsonGPIO.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#define NUM_THREADS 8;

using namespace std;

jetsonTX1GPIONumber GPIOs[NUM_THREADS] = [gpio187, gpio219, gpio38, gpio63, gpio36, gpio37, gpio160, gpio161];

thread threads[NUM_THREADS];
int fanData[NUM_THREADS];
mutex fanMutex[NUM_THREADS];
condition_variable fanCV[NUM_THREADS];

int main() {
	int i;
	for(i=0; i<NUM_THREADS; i++) {
		threads[i] = thread(fanThread, i)
	}
	
	while(true) {
		for(i=0; i<NUM_THREADS; i++) {
			{
				fanData[i] = 1000; //set data
				lock_guard<std::mutex> lock(fanMutex[i]);  //acquire lock
			}
			fanCV[i].notify_one(); //notify one
			
		}
		
	}
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
    cv.notify_one();
	

}

void TurnFanOn(int milliseconds, int gpio) {
	cout << "Turning fan on" << endl;
	gpioSetValue(GPIOs[gpio], on);
	usleep(milliseconds);         
	cout << "Turning fan off" << endl;
	gpioSetValue(GPIOs[gpio], off);
}

