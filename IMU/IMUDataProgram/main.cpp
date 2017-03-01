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
	float dx, dy, dz =0 ;
	float oldax, olday, oldaz =0;
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
				float ax = (imuData.accel.x())*9.8;
				float ay = (imuData.accel.y())*9.8;
				float az = (imuData.accel.z())*9.8;
				dx = dx + .5*(ax-oldax)*.1*.1;
				dy = dy + .5*(ay-olday)*.1*.1;
				dz = dz + .5*(az-oldaz)*.1*.1;
				printf("Sample Accelerometers %d: X:%f, Y:%f, Z:%f\r", sampleRate, dx, dy, dz);
				fflush(stdout);
				//fflush(stdout);
				oldax=ax;
				olday=ay;
				oldaz=az;
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
