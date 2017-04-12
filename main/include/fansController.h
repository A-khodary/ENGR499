#ifndef FANSCONTROLLER
#define FANSCONTROLLER

// Since all these functions are local,
// don't have to make them public to other files

/*
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

void checkRotation();
void TurnFanOn(int onPercent, int i);
void TurnFanOff(int i);
void fanThread(int i);
void wakeUpFanThread(int i);
void powerDown(int sig);
void moveForward(int onPercent);
void moveBackward(int onPercent);
void moveLeft(int onPercent);
void moveRight(int onPercent);
void turnCCW(int onPercent);
void turnCW(int onPercent);
*/

void FanExecution();

#endif

