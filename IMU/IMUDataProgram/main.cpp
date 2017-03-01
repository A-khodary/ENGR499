//Program that displays IMU Data on Command Line
//Uses the RTIMULib

#include "RTIMULib.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	//Creates .ini File
	RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
	//Detects IMU type and creates connection
	RTIMU *imu = RTIMU::createIMU(settings);
	if((imu== NULL) || (imu->IMUType()==RTIMU_TYPE_NULL))
	{
		printf("No IMU found, exiting program \n");
		exit(1);
	}
	
	//Set UP IMU
	imu->IMUInit();
	//Enable Fusion
	imu->setSlerpPower(0.02);
	imu->setGyroEnable(true);
	imu->setAccelEnable(true);
	imu->setCompassEnable(true);
	//Set Rate Timer
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t startTimer;
	startTimer = rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

	//Process Data
	int sampleCount=0;
	int sampleRate=0;
	uint64_t now;
	float dx, dy, dz =0 ;
	float oldax=0, olday=0, oldaz=0, old2ax=0, old2ay=0, old2az=0, firstx=0, firsty=0, firstz=0;
	bool firstRead =false;
	ofstream dataFile;
	dataFile.open("data.csv");
	dataFile << "AccelX,AccelY,AccelZ,OldAX,OldAY,OldAZ,Old2AX,Old2AY,Old2AZ,AvgAX,AvgAY,AvgAZ,DX,DY,Dz" << endl;
	while(1)
	{
		//Poll at recommended IMU rate
		usleep(imu->IMUGetPollInterval() *1000);
		while(imu->IMURead())
		{
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			if((now-startTimer)>5000000)
			{
				//Display Rate
				if((now-displayTimer)>100000)
				{	
					//printf("Sample Degree %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
					//printf("Sample Radian %d: %s\r", sampleRate, RTMath::displayRadians("", imuData.fusionPose));
					float ax = (imuData.fusionPose.x())*9.8;
					float ay = (imuData.fusionPose.y())*9.8;
					float az = (imuData.fusionPose.z())*9.8;
					float avgax = (ax+oldax +old2ax)/3;
					float avgay = (ay+olday +old2ay)/3;
					float avgaz = (az+oldaz +old2az)/3;	

					if(!firstRead)
					{
						firstRead=true;
						firstx=ax;
						firsty=ay;
						firstz=az;
					}

					dx = dx + .5*(avgax-firstx)*.1*.1;
					dy = dy + .5*(avgay-firsty)*.1*.1;
					dz = dz + .5*(avgaz-firstz)*.1*.1;

					dataFile << ax<<","<<ay<<","<<az << "," 
						<< oldax << "," << olday << "," << oldaz << ","
						<< old2ax << "," << old2ay << "," << old2az << ","
						<< avgax-firstx << "," << avgay-firsty << "," << avgaz-firstz << ","
						<< dx << "," << dy << "," << dz << endl;

					printf("Sample Accelerometers %d: X:%f, Y:%f, Z:%f\r", sampleRate, dx, dy, dz);
					fflush(stdout);

					//fflush(stdout);
					oldax=ax;
					olday=ay;
					oldaz=az;
					old2ax=oldax;
					old2ay=olday;
					old2az=oldaz;
					displayTimer = now;
				}
				if((now-rateTimer)>1000000)
				{
					sampleRate=sampleCount;
					sampleCount=0;
					rateTimer = now;
				}
			}
		}	
	}
	dataFile.close();	
}
