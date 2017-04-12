//Program that displays IMU Data on Command Line
//Uses the RTIMULib

#include "RTIMULib.h"
#include "RTFusionRTQF.h"
#include "imu.h"

#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

void
IMUExecution()
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
  dataFile << "raX,raY,raZ,arX,arY,arZ,vrX,vrY,vrZ,prX,prY,prZ,gyroX,gyroY,gyroZ,compassX,compassY,compassZ" << endl;
  RTFusionRTQF fusion;


  float posresid[3] ={0,0,0};
  float velresid[3] ={0,0,0};

  float prevposresid[3] ={0,0,0};
  float prevvelresid[3] ={0,0,0};
  float prevaccelresid[3] ={0,0,0};
  printf("%ld,%ld \n",now,startTimer);

  float initaccelsum[3]={0,0,0};
  while((now-startTimer)<40000000)
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
	  if((now - displayTimer) > 50000)
	    {
	      displayTimer=now;

	      RTFLOAT rG[3];
	      RTFLOAT mA[3];
	      RTVector3 accel = imu->getAccel();
	      RTVector3 accelResid = fusion.getAccelResiduals();
	      float modifiedAccel[3]={0,0,0};
	      for(int i=0;i<3;++i)
		{

		  //pos[i]=pos[i]+velocity[i]*.1+.5*.01*((int)(mA[i]/.001)*.001);
		  //posresid[i]=posresid[i]+velresid[i]*.1+.5*.01*((int)(accelResid.data(i)/.001)*.001);
		  //pos[i]=pos[i]+velocity[i]*.1+.5*.01*(mA[i]);
		  //posresid[i]=posresid[i]+velresid[i]*.1+.5*.01*(accelResid.data(i));
		  //velocity[i]=velocity[i]+mA[i]*.1;
		  //velresid[i]=velresid[i]+accelResid.data(i)*.1;
		  float tempaccel = accelResid.data(i)-initaccelsum[i]/50000;
					
		  //Computing Velocity
		  if(abs(tempaccel)>.001)
		    modifiedAccel[i]=tempaccel;
		  //float avgAccelresid = .5*(prevaccelresid[i]+accelResid.data(i));
		  float avgAccelresid = .5*(prevaccelresid[i]+modifiedAccel[i]);
		  float deltaV = deltaTime*avgAccelresid;
		  velresid[i] = prevvelresid[i] + deltaV;
			
		  //Computing Position
		  float avgVelresid = .5*(prevvelresid[i]+velresid[i]);
		  float deltaP = deltaTime*avgVelresid;
		  posresid[i] = prevposresid[i] + deltaP;

		  //Saving Values for next time step
		  //prevaccelresid[i] = accelResid.data(i);
		  prevaccelresid[i] = modifiedAccel[i];
		  prevvelresid[i]=velresid[i];
		  prevposresid[i]=posresid[i];

		}
	      RTVector3 gyro = imu->getGyro();
	      RTVector3 compass = imu->getCompass();
	      dataFile << accel.data(0) << "," << accel.data(1)<< "," << accel.data(2) << ","  << modifiedAccel[0] <<"," << modifiedAccel[1] << "," << modifiedAccel[2]<< "," << velresid[0] << "," << velresid[1] << "," << velresid[2] << "," << posresid[0] << "," << posresid[1] << "," << posresid[2] << "," << gyro.data(0) << "," << gyro.data(1) << "," << gyro.data(2) << "," << compass.data(0) << "," << compass.data(1) << "," << compass.data(2)<< "," << now << "," << prevnow<< "," << deltaTime << endl;
	      //fflush(stdout);
	    }
	  else
	    {
	      RTVector3 accelResid = fusion.getAccelResiduals();
	      for(int i=0; i<3; ++i)
		{
		  initaccelsum[i]+=accelResid.data(i);
		}
	      printf("Reading: %ld\r", (now - displayTimer));
	      fflush(stdout);
	    }
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
