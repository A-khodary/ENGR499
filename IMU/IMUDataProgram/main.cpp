//Program that displays IMU Data on Command Line
//Uses the RTIMULib

#include "RTIMULib.h"

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
	rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

	//Process Data
	int sampleCount=0;
	int sampleRate=0;
	uint64_t now;
	while(1)
	{
		//Poll at recommended IMU rate
		usleep(imu->IMUGetPollInterval() *1000);
		while(imu->IMURead())
		{
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;
			now = RTMath::currentUSecsSinceEpoch();
			//Display Rate
			if((now-displayTimer)>100000)
			{
				//printf("Sample Degree %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
				//printf("Sample Radian %d: %s\r", sampleRate, RTMath::displayRadians("", imuData.fusionPose));
				printf("Sample Accelerometers %d: X:%f, Y:%f, Z:%f\r", sampleRate, imuData.accel.x(), imuData.accel.y(), imuData.accel.z());
				fflush(stdout);
				//fflush(stdout);
				displayTimer = now;
			}
			if((now-rateTimer)>100000)
			{
				sampleRate=sampleCount;
				sampleCount=0;
				rateTimer = now;
			}
		}	
	}	
}
