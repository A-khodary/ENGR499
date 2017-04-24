/*
 * Main file for starting threads
 */
#include "camera.h"
#include "qr_camera.h"
#include "shape_camera.h"
#include "imu.h"
#include "fansController.h"
#include "bluetooth.h"

#include <mutex>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <condition_variable>

extern void moveForward(int onpercent);
extern void moveBackward(int onpercent);

const int cameraDevice = 0;

void RunBluetooth(std::string myAddr)
{
    ExecuteBluetooth(myAddr);
}

void RunQRCamera(Camera* camera)
{
    QRCamera qrCamera(camera);

    while (true)
    {
	//std::cout << "Cheese" << std::endl;
	cv::Mat img = qrCamera.TakePicture();
	qrCamera.decodeFrame(img);
	cv::waitKey(100);
    }
}

void RunShapeCamera(Camera* camera)
{
    ShapeCamera shapeCamera(camera);

    while (true)
    {
	cv::Mat shapeImg = shapeCamera.TakePicture();
	shapeImg = shapeCamera.ThresholdImage(shapeImg);
	shapeCamera.RecognizeShapes(shapeImg);

	cv::waitKey(100);
    }
}

void RunIMU(struct imuInfo &data, std::mutex &imuDataMutex, std::condition_variable &cv)
{
    IMUExecution(data, imuDataMutex, cv);
}

void RunFans()
{
    FanExecution();
}

void ReadData(struct imuInfo &data, std::mutex &imuDataMutex, std::condition_variable &cv)
{
    std::unique_lock<std::mutex> datalock(imuDataMutex);
    datalock.unlock();
    float desiredx=0;
    float desiredy=0;
    float desiredvx =0;
    float desiredvy =0;
    float desiredpitch =0;
    float kp =5000;
    float kd=1;
    while(1){
		
    	datalock.lock();
	cv.wait(datalock);
	float errorvx = desiredvx - data.velx;
	float controlvx = kp * errorvx;
	float errorx = 	desiredx - data.x;
	float controlx = kp * errorx + kd*controlvx;

	float errorvy = desiredvy - data.vely;
	float controlvy = kp * errorvy;
	float errory = 	desiredy - data.y;
	float controly = kp * errory + kd*controlvy;

	float errorpitch = desiredpitch-data.pitch;
	float controlpitch = kp*errorpitch + kd*controlpitch;
	//printf("VelX:%lf,PosX:%lf,VelY%lf,PosY:%lf,Pitch:%lf\n",data.velx,data.x,data.vely,data.y,data.pitch);
	printf("ControlX:%lf,ControlY:%lf,ControlP:%lf \n",controlx, controly, controlpitch);
	fflush(stdout);
	if(controlx<0)
	    moveForward(controlx);
	else
	    moveBackward(controlx);
	cv.notify_one();
    	datalock.unlock();
		
    }
}


int main(int argc, char* argv[])
{
    imuInfo imuData;
    imuData.x=0;
    imuData.y=0; 
    imuData.velx=0;
    imuData.vely=0; 
    imuData.roll=0; 
    imuData.pitch=0; 
    imuData.yaw=0;
    std::mutex imuDataMutex;
    std::condition_variable writeDatacv;
    // Global camera object referenced by camera threads
    /*Camera* camera = new Camera();
      if (!camera->OpenVideoCap(cameraDevice))
      {
      std::cout << "Video capture is not opened" << std::endl;
      return 1;
      }
    */

    
    // Start the Bluetooth thread
    std::thread bluetoothThread(RunBluetooth, argv[1]);
	
    // Start the QR camera thread
    //std::thread qrCameraThread(RunQRCamera, camera);

    // Start the Shape camera thread
    //std::thread shapeCameraThread(RunShapeCamera, camera);
    
    // Start the IMU thread
    //std::thread imuThread(&RunIMU, std::ref(imuData),
    //			  std::ref(imuDataMutex),std::ref(writeDatacv));
    //std::thread readDataThread(&ReadData, std::ref(imuData),
    //			       std::ref(imuDataMutex),
    //			       std::ref(writeDatacv));
    
    // Start the fan thread
    //std::thread fanThread(RunFans);

    bluetoothThread.join();
    //qrCameraThread.join();
    //shapeCameraThread.join();
    //imuThread.join();
    //readDataThread.join();
    //fanThread.join();

    //delete camera;
    
    return 0;
}
