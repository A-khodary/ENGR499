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
	uint64_t now;
	uint64_t prevnow;
	prevnow=now=startTimer = rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

	//Process Data
	int sampleCount=0;
	int sampleRate=0;


	ofstream dataFile;
	dataFile.open("data.csv");
	dataFile << "raX,raY,raZ,mAX,mAY,mAz,vX,vY,vZ,pX,pY,pZ,arX,arY,arZ,vrX,vrY,vrZ,prX,prY,prZ,gyroX,gyroY,gyroZ,compassX,compassY,compassZ" << endl;
	RTFusionRTQF fusion;
	float gravity[3]={0,0,1.0};
	//float pos[3] ={0,0,0};
	float velocity[3] ={0,0,0};
	float position[3] ={0,0,0};

	float posresid[3] ={0,0,0};
	float velresid[3] ={0,0,0};

	float prevposition[3] ={0,0,0};
	float prevvelocity[3] ={0,0,0};
	float prevacceleration[3] ={0,0,0};	

	float prevposresid[3] ={0,0,0};
	float prevvelresid[3] ={0,0,0};
	float prevaccelresid[3] ={0,0,0};
	printf("%ld,%ld \n",now,startTimer);
	
	while((now-startTimer)<300000000)
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
			float deltaTime =(now-prevnow)/1000000.0;
			//if((now - displayTimer) > 50000)
			//{
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
				RTFLOAT mA[3];
				RTVector3 accel = imu->getAccel();
				RTVector3 accelResid = fusion.getAccelResiduals();
				float modifiedAccel[3]={0,0,0};
				for(int i=0;i<3;++i)
				{
					rG[i]=gravity[0]*R[0][i]+gravity[1]*R[1][i]+gravity[2]*R[2][i];
					mA[i]=accel.data(i)-rG[i];
					//pos[i]=pos[i]+velocity[i]*.1+.5*.01*((int)(mA[i]/.001)*.001);
					//posresid[i]=posresid[i]+velresid[i]*.1+.5*.01*((int)(accelResid.data(i)/.001)*.001);
					//pos[i]=pos[i]+velocity[i]*.1+.5*.01*(mA[i]);
					//posresid[i]=posresid[i]+velresid[i]*.1+.5*.01*(accelResid.data(i));
					//velocity[i]=velocity[i]+mA[i]*.1;
					//velresid[i]=velresid[i]+accelResid.data(i)*.1;
					
					
					//Computing Velocity
					if(abs(accelResid.data(i))>.001)
						modifiedAccel[i]=accelResid.data(i);
					//float avgAccelresid = .5*(prevaccelresid[i]+accelResid.data(i));
					float avgAccelresid = .5*(prevaccelresid[i]+modifiedAccel[i]);
					float deltaV = deltaTime*avgAccelresid;
					velresid[i] = prevvelresid[i] + deltaV;

					float avgAcceleration = .5*(prevacceleration[i]+mA[i]);
					float deltaV2 = deltaTime*avgAcceleration;
					velocity[i] = prevvelocity[i] + deltaV2;
					
					//Computing Position
					float avgVelresid = .5*(prevvelresid[i]+velresid[i]);
					float deltaP = deltaTime*avgVelresid;
					posresid[i] = prevposresid[i] + deltaP;

					float avgVelocity = .5*(prevvelocity[i]+velocity[i]);
					float deltaP2 = deltaTime*avgVelocity;
					position[i] = prevposition[i] + deltaP2;
					//Saving Values for next time step
					//prevaccelresid[i] = accelResid.data(i);
					prevaccelresid[i] = modifiedAccel[i];
					prevvelresid[i]=velresid[i];
					prevposresid[i]=posresid[i];
					
					prevacceleration[i] = mA[i];
					prevvelocity[i]=velocity[i];
					prevposition[i]=position[i];
				}
				RTVector3 gyro = imu->getGyro();
				RTVector3 compass = imu->getCompass();
				//printf("Sample Accelerometers %d: aX:%f, aY:%f, aZ:%f\r", sampleRate, accelResid.data(0), accelResid.data(1), accelResid.data(2));
				dataFile << accel.data(0) << "," << accel.data(1)<< "," << accel.data(2) << "," << mA[0] << "," << mA[1] << "," << mA[2]<< ","<< velocity[0] << "," << velocity[1] << "," << velocity[2] << "," << position[0] << "," << position[1] << "," << position[2] << "," << modifiedAccel[0] <<"," << modifiedAccel[1] << "," << modifiedAccel[2]<< "," << velresid[0] << "," << velresid[1] << "," << velresid[2] << "," << posresid[0] << "," << posresid[1] << "," << posresid[2] << "," << gyro.data(0) << "," << gyro.data(1) << "," << gyro.data(2) << "," << compass.data(0) << "," << compass.data(1) << "," << compass.data(2)<< "," << now << "," << prevnow<< "," << deltaTime << endl;
				//fflush(stdout);
			//}
			prevnow=now;
			if((now - rateTimer) > 1000000)
			{
				sampleRate=sampleCount;
				sampleCount=0;
				rateTimer = now;
			}
		}	
	}
	dataFile.close();	
}
