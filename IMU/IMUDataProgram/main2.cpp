//Program that displays IMU Data on Command Line
//Uses the RTIMULib

#include "RTIMULib.h"
#include <iostream>
#include <fstream>
#include <cmath>
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
	uint64_t now=0;

	ofstream dataFile;
	dataFile.open("data.csv");
	dataFile << "GyroX,GyroY,GyroZ,AccelX,AccelY,AccelZ,FusionX,FusionY,FusionZ,IntegratedX,IntegratedY,IntegratedZ" << endl;
	float paX=0, paY=0, paZ=0; //Previous aX
	float pdeltaX=0, pdeltaY=0, pdeltaZ=0;
	float iax=0, iay=0, iaz=0;
	while(1)
	{
		//Poll at recommended IMU rate
		usleep(imu->IMUGetPollInterval() *1000);
		while(imu->IMURead())
		{
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			if((now-startTimer)>3)
			{
				//Display Rate
				if((now-displayTimer)>100000)
				{	
					//printf("Sample Degree %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
					//printf("Sample Radian %d: %s\r", sampleRate, RTMath::displayRadians("", imuData.fusionPose));
					RTVector3 test;
					RTVector3 cP;
					test.setX(imuData.fusionQPose.x());
					test.setY(imuData.fusionQPose.y());
					test.setZ(imuData.fusionQPose.z());
					cP.crossProduct(test, imuData.accel, cP);
					float s =imuData.fusionQPose.scalar();
					RTVector3 resultTest = 2*test.dotProduct(test,imuData.accel)*imuFusionQPose+(s*s-test.dotProduct(test,imuData.accel)*imuData.accel+2*s*cP;
				
					//imuData.fusionQPose.toEuler(test);
					float fx=test.x();
					float fy=test.y();
					float fz=test.z();
					//float fx = 2*(imuData.fusionQPose.x()*imuData.fusionQPose.z()-imuData.fusionQPose.scalar()*imuData.fusionQPose.y());
					//float fy = 2*(imuData.fusionQPose.scalar()*imuData.fusionQPose.x()+imuData.fusionQPose.y()*imuData.fusionQPose.z());
					//float fz = (imuData.fusionQPose.scalar()*imuData.fusionQPose.scalar()-imuData.fusionQPose.x()*imuData.fusionQPose.x()-imuData.fusionQPose.y()*imuData.fusionQPose.y()+imuData.fusionQPose.z()*imuData.fusionQPose.z());
					float ax = (imuData.accel.x()-fx)*9.8;
					float ay = (imuData.accel.y()-fy)*9.8;
					float az = (imuData.accel.z()-fz)*9.8;
					float gx = imuData.gyro.x();
					float gy = imuData.gyro.y();
					float gz = imuData.gyro.z();
					float deltaX = (ax-paX)/.1;
					float deltaY = (ay-paY)/.1;
					float deltaZ = (az-paZ)/.1;
					iax=iax+.05*(pdeltaX+deltaX);
					iay=iay+.05*(pdeltaY+deltaY);
					iaz=iaz+.05*(pdeltaZ+deltaZ);
					/*
					dataFile << ax<<","<<ay<<","<<az << "," 
						<< oldax << "," << olday << "," << oldaz << ","
						<< old2ax << "," << old2ay << "," << old2az << ","
						<< avgax-firstx << "," << avgay-firsty << "," << avgaz-firstz << ","
						<< dx << "," << dy << "," << dz << endl;
					*/
					dataFile << gx << "," << gy << "," << gz << "," << ax << "," << ay << "," << az << "," << fx << "," << fy << "," << fz << "," << iax << "," << iay << "," << iaz << endl;
					printf("Sample Accelerometers %d: aX:%f, aY:%f, aZ:%f\r", sampleRate, fx, fy, fz);
					fflush(stdout);
					pdeltaX=deltaX;
					pdeltaY=deltaY;
					pdeltaZ=deltaZ;
					paX=ax;
					paY=ay;
					paZ=az;
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
