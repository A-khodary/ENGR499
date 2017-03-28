//Program that displays IMU Data on Command Line
//Uses the RTIMULib

#include "RTIMULib.h"
#include "RTFusionRTQF.h"
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
	dataFile << "AccelX,AccelY,AccelZ,PosX,PosY,PosZ,rGX,rGY,rGZ" << endl;
	RTFusionRTQF fusion;
	float gravity[3]={0,0,1.0};
	float pos[3] ={0,0,0};
	float posresid[3] ={0,0,0};
	while(1)
	{
		//Poll at recommended IMU rate
		usleep(imu->IMUGetPollInterval() *1000);
		while(imu->IMURead())
		{
			RTIMU_DATA imuData = imu->getIMUData();
			fusion.newIMUData(imuData, settings);
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			//Display Rate
			if((now-displayTimer)>100000)
			{
				displayTimer=now;
				RTQuaternion qPose = fusion.getMeasuredQPose();
				RTFLOAT R[3][3];
				R[0][0] = qPose.data(0)*qPose.data(0)+qPose.data(1)*qPose.data(1)-qPose.data(2)*qPose.data(2)-qPose.data(3)*qPose.data(3);
				R[0][1] = 2*(qPose.data(1)*qPose.data(2)-qPose.data(0)*qPose.data(3));
				R[0][2] = 2*(qPose.data(1)*qPose.data(3)+qPose.data(0)*qPose.data(2));
				R[1][0] = 2*(qPose.data(1)*qPose.data(2)+qPose.data(0)*qPose.data(3));
				R[1][1] = qPose.data(0)*qPose.data(0)-qPose.data(1)*qPose.data(1)+qPose.data(2)*qPose.data(2)-qPose.data(3)*qPose.data(3);
				R[1][2] = 2*(qPose.data(2)*qPose.data(3)-qPose.data(0)*qPose.data(1));
				R[2][0] = 2*(qPose.data(1)*qPose.data(3)-qPose.data(0)*qPose.data(2));
				R[2][1] = 2*(qPose.data(2)*qPose.data(3)+qPose.data(0)*qPose.data(1));
				R[2][2] = qPose.data(0)*qPose.data(0)-qPose.data(1)*qPose.data(1)-qPose.data(2)*qPose.data(2)+qPose.data(3)*qPose.data(3);
				
				RTFLOAT rG[3];
				rG[0]=gravity[0]*R[0][0] +gravity[1]*R[1][0] +gravity[2]*R[2][0];
				rG[1]=gravity[0]*R[0][1] +gravity[1]*R[1][1] +gravity[2]*R[2][1];
				rG[2]=gravity[0]*R[0][2] +gravity[1]*R[1][2] +gravity[2]*R[2][2];

				RTFLOAT mA[3];
				RTVector3 accel = imu->getAccel();
				RTVector3 accelResid = fusion.getAccelResiduals();
				for(int i=0;i<3;++i)
				{
					mA[i]=accel.data(i)-rG[i];
					pos[i]=pos[i]+.5*.01*((int)(mA[i]/.001)*.001);
					posresid[i]=posresid[i]+.5*.01*((int)(accelResid.data(i)/.001)*.001);
				}
				printf("Sample Accelerometers %d: aX:%f, aY:%f, aZ:%f\r", sampleRate, accelResid.data(0), accelResid.data(1), accelResid.data(2));
				dataFile << mA[0] << "," << mA[1] << "," << mA[2]<< "," << pos[0] << "," << pos[1] << "," << pos[2] << "," << accelResid.data(0) <<"," << accelResid.data(1) << "," << accelResid.data(2)<< "," << posresid[0] << "," << posresid[1] << "," << posresid[2] << endl;
				fflush(stdout);
			}
			if((now-rateTimer)>1000000)
			{
				sampleRate=sampleCount;
				sampleCount=0;
				rateTimer = now;
			}
		}	
	}
	dataFile.close();	
}
